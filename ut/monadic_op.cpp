#include "results.h"
#include <gtest/gtest.h>
#include <string>

namespace
{

auto foo(int a, int b) -> result<int, int> {
  if (a < b) {
    return ok(a + b);
  } else {
    return failed(a - b);
  }
}

auto bar(std::string_view s1, char ch) -> result<std::string, int> {
  auto it = std::find(s1.begin(), s1.end(), ch);
  if (it == s1.end()) {
    return ok(std::string{s1});
  }
  return failed(-1);
}

struct error_case {
  explicit error_case(std::string_view e) : error{e} {

  }

  std::string error;
};

auto operator << (std::ostream& os, const error_case e) -> std::ostream& {
  return os << "error: [" << e.error << "]";
}

enum good_result {
  OK, EVEN_BETTER, BEST
};


auto f2(std::string_view s) -> result<std::string_view, error_case> {
  if (s.empty()) {
    return failed(error_case{"empty string!"});
  }
  if (s.size() > 10) {
    return failed(error_case{"this string is too long"});
  }
  return ok(s);
}

}  // end of local namespace

TEST(MonadicOpTest, TestElseCase) {
    // for these tests we are applying the else case, meaning if failed than call
    // the operation that we are passing to process the error

    const int b = 10, a = 20;
    const int factor = 5;
    const auto r = foo(a, b).or_else([factor](auto e) -> result<int, int> {
        return ok(e * factor);
    });
    ASSERT_TRUE(r.is_ok());
    const auto expected = (a - b) * factor;
    ASSERT_EQ(r.unwrap(), expected) << "the else function change the value to "  << expected <<", but we have " << r.unwrap();

    // now lets see what happens when we are not failing
    const auto r2 = foo(b, a).or_else([factor](auto e) -> result<int, int> {
        return ok(e * factor);
    });
    ASSERT_TRUE(r2.is_ok());    // we were successful, that's true
    ASSERT_EQ(r2.unwrap(), (a + b)) << "this time we should not be getting into the else case: " << r.unwrap();
}

TEST(MonadicOpTest, TestElseCaseComplex) {
    // in this case we will up the antis, we will have the error and ok types different
    const auto r = bar("some string", ' ').or_else([](auto&& e) -> result<std::string, error_case> {
        // the error type here is int, lets convert this to enum
        return failed(error_case("I didn't like what I see here: " + std::to_string(e)));
    });
    ASSERT_TRUE(r.is_error()) << "since we pass through the else case, it must failed!";
    ASSERT_EQ(r.error_value().error.substr(0, 18), "I didn't like what") << "we got " << r;
    // and lets see when we are not failing what will happen
    const auto r2 = bar("some string", '+').or_else([](auto&& e) -> result<std::string, error_case> {
        // the error type here is int, lets convert this to enum
        return failed(error_case("I didn't like what I see here: " + std::to_string(e)));
    });
    ASSERT_TRUE(r2.is_ok()) << "this first call should be fine, so we're good";
    ASSERT_EQ(r2.unwrap(), "some string");

    const std::string err_str = "this is an error";
    const std::string_view ok_case = "good";
    const auto r3 = f2("this is a string").or_else([err_str, ok_case](auto&& ec) -> result<std::string_view, std::string> {
        if (ec.error.empty()) {
            return ok(ok_case);
        }
        return failed(err_str);
    });
    ASSERT_TRUE(r3.is_error()) << "we must failed since we are in the else case";
    //ASSERT_EQ(r3.error_value(), err_str) << "expected " << err_str << ", but got: <" << r3.error_value() << ">";
}

TEST(MonadicOpTest, CheckUnwrapOr) {
    const std::string expected_value = "expected";

    const auto r = bar("this is a string", ' ').unwrap_or(expected_value);  // since we failed here, we are should go into the _or case
    ASSERT_EQ(expected_value, r);

    const auto r2 = bar("this is another string", '+').unwrap_or(expected_value);
    ASSERT_NE(r2, expected_value);  // in this case we didn't get into the error case
}

TEST(MonadicOpTest, TestThenCaseComplex) {
    // this will cover the cases where we are running an operation that
    // after we are successful
    const auto r = bar("some string", '+').and_then([](auto&& e) -> result<std::string, int> {
        // the error type here is int, lets convert this to enum
        return failed(int(e.size()));
    });
    ASSERT_TRUE(r.is_error()) << "since we are going into the and then and we are failing in it, it must be an error";
    ASSERT_EQ(r.error_value(), int(std::string("some string").size()));

    const auto r2 = f2("hello").and_then([](auto&& s) -> result<int, error_case> {
        if (s.size() < 10) {
            return ok(10);
        } else {
            return failed(error_case("this is bad"));
        }
    });

    ASSERT_TRUE(r2.is_ok()) << "should pass all ok tests";
    ASSERT_EQ(r2.unwrap(), 10);
}
