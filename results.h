#pragma once
#include <variant>
#include <optional>
#include <iostream>
#include <sstream>
#include <functional>

namespace internal
{

template <typename T, const bool POD> 
struct basic_result_type {
    using type = T;
    using arg_type = T;

    constexpr basic_result_type(arg_type v) : value{std::move(v)} {

    }

    type value = {};
};

template<typename T>
struct basic_result_type<T, true> {
    using type = T;
    using arg_type = T;

    constexpr basic_result_type(arg_type v) : value{v} {

    }

    type value = {};
};

template<typename T>
struct simple_type {
	static constexpr bool value = std::is_fundamental<T>::value || 
				std::is_pod_v<T> || 
				std::is_pointer<T>::value;
};

}	// end of namespace internal

template<typename T>
struct success : internal::basic_result_type<T, internal::simple_type<T>::value> {
                                
    using base_t = internal::basic_result_type<T, internal::simple_type<T>::value>;

    using type = typename base_t::type;

    constexpr success(typename base_t::arg_type input) : base_t{input} {

    }

    constexpr operator const T& () const {
	    return base_t::value;
    }

    constexpr operator T& () {
	    return base_t::value;
    }

    constexpr operator T&& () {
	    return base_t::value;
    }

};

template<typename T>
[[nodiscard]] inline constexpr auto ok(T v) -> success<T> {
	return success<T>{v};
}

template<typename T>
struct failure : internal::basic_result_type<T, internal::simple_type<T>::value> {
    using base_t = internal::basic_result_type<T, internal::simple_type<T>::value>;
                            
    using type = typename base_t::type;

    constexpr failure(typename base_t::arg_type input) : base_t{input} {

    }

    constexpr operator const T& () const {
	    return base_t::value;
    }

    constexpr operator T& () {
	    return base_t::value;
    }

    constexpr operator T&& () {
	    return base_t::value;
    }
};

template<typename T>
[[nodiscard]] inline constexpr auto failed(T v) -> failure<T> {
	return failure<T>{v};
}

// Define a result type, which means this is either type -
// we either have a success value, or we have an error value
// unlike with functional style types we cannot pattern match this
// since C++ don't have it
template<typename OK, typename ERR>
struct result
{

    using ok_type = success<OK>;
    using error_type = failure<ERR>;
    using success_t = typename ok_type::type;
    using failure_t = typename error_type::type;

    constexpr result(ok_type&& happy_path) : value{std::move(happy_path)} {

    }

    constexpr result(error_type&& failure) : value{std::move(failure)} {

    }

    [[nodiscard]] constexpr auto is_ok() const -> bool {
        return std::holds_alternative<ok_type>(value);
    }

    [[nodiscard]] constexpr auto is_error() const -> bool {
        return !is_ok();
    }

    // unsafe access to the "good" result
    // it is better to first check whether we have this
    [[nodiscard]] constexpr auto unwrap() const -> const typename ok_type::type& {
        return std::get<ok_type>(value).value;
    }
    [[nodiscard]] constexpr auto unwrap() -> typename ok_type::type&& {
        return std::get<ok_type>(value).value;
    }

    // This is a "safe" version of unwrap, by this we mean that, unlike the
    // unwrap version, that calling it when we have the error case, which can result
    // in an exception, in this case, if we don't have the success value, then we will
    // return some default value
    [[nodiscard]] constexpr auto unwrap_or(const typename ok_type::type& some_default) const -> const typename ok_type::type& {
        if (is_ok()) {
            return unwrap();
        }
        return some_default;
    }

    [[nodiscard]] constexpr auto unwrap_or(typename ok_type::type&& some_default) -> typename ok_type::type&& {
        if (is_ok()) {
            return unwrap();
        }
        return some_default;
    }

    [[nodiscard]] constexpr explicit  operator bool() const {
        return is_ok();
    }

    // unsafe access to the error value
    [[nodiscard]] constexpr auto error_value() const -> const typename error_type::type& {
        return std::get<error_type>(value).value;
    }

    [[nodiscard]] constexpr  auto error_value() -> typename error_type::type&& {
        return std::get<error_type>(value).value;
    }

    // in case we have successful result, run then_f
    template<typename F>
    [[nodiscard]] auto and_then(F&& then_f) -> result<typename std::result_of<F(success_t)>::type::success_t,  failure_t> {
        using new_success_t = typename std::result_of<F(failure_t)>::type::success_t;
        using expected_t = typename std::result_of<F(failure_t)>::type::failure_t;
        static_assert(std::is_same_v<expected_t, failure_t>);       // make sure that we are keeping the same error type, we cannot change it here
        using new_type = result<new_success_t, failure_t>;

        if (is_error()) {
            return new_type{std::move(std::get<error_type>(value))};
        }
        return new_type{std::invoke(then_f, std::move(std::get<ok_type>(value).value))};
    }

    // in case of an error, run the else_f
    template<typename F>
    [[nodiscard]] auto or_else(F&& else_f) -> result<success_t, typename std::result_of<F(failure_t)>::type::failure_t> {
        using new_err_t = typename std::result_of<F(failure_t)>::type::failure_t;
        using expected_t = typename std::result_of<F(failure_t)>::type::success_t;

        static_assert(std::is_same_v<expected_t, success_t>);   // make sure that the function passed here return the correct result type

        using new_type = result<success_t, new_err_t>;

        if (is_error()) {
            return new_type{std::invoke(else_f, std::move(std::get<error_type>(value).value))};
        }
        return new_type{std::move(std::get<ok_type>(value))};
    }

    // map an error to another error, or return the new type of result unaffected in case it didn't fail
    template<typename F>
    [[nodiscard]] auto map_err(F&& map_f)  -> result<success_t, typename std::result_of<F(failure_t)>::type::failure_t> {
        using new_err_t = typename std::result_of<F(failure_t)>::type::failure_t;
        using expected_t = typename std::result_of<F(failure_t)>::type::success_t;
        static_assert(std::is_same_v<expected_t, success_t>);   // make sure that the function passed here return the correct result type
        using new_type = result<success_t, new_err_t>;

        if (is_error()) {
            return new_type{std::invoke(map_f, std::move(std::get<error_type>(value).value))};
        }
        return new_type(std::move(std::get<ok_type>(value)));
    }

private:
    using value_type = std::variant<ok_type, error_type>;
    
    value_type value;
};

// safe access to the either error or ok
// a good pattern for using this is:
// auto my_result = ..
// if (auto ok_val = Ok(my_result); ok_val) { .. // handle the good path
template<typename O, typename E>
[[nodiscard]] inline constexpr auto Ok(const result<O, E>& r) -> std::optional<O> {
    if (r) {
        return r.unwrap();
    }
    return {};
}

// mutable access
template<typename O, typename E>
[[nodiscard]] inline constexpr auto Ok(result<O, E>& r) -> std::optional<O> {
    if (r) {
        return r.unwrap();
    }
    return {};
}

// extract the error
template<typename O, typename E>
[[nodiscard]] inline constexpr auto Error(const result<O, E>& r) -> std::optional<E> {
    if (!r) {
        return r.error_value();
    }
    return {};
}

template<typename O, typename E>
[[nodiscard]] inline constexpr auto Error(result<O, E>& r) -> std::optional<E> {
    if (!r) {
        return r.error_value();
    }
    return {};
}

template<typename O, typename E>
inline auto operator << (std::ostream& os, const result<O, E>& r) -> std::ostream& {
    if (r.is_error()) {
        return os<<"error: "<<r.error_value();
    } else {
        return os<<"success: " << r.unwrap();
    }
}

template<typename Good, typename Err>
inline auto to_string(const result<Good, Err>& from) -> std::string {
    std::ostringstream formatter;
    formatter << from;
    return formatter.str();
}
