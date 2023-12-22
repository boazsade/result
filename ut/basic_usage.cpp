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

auto f(good_result gr) -> result<good_result, int> {
  switch (gr) {
    case good_result::BEST:
      return failed(1);
    case good_result::OK:
      return ok(gr);
    default:
      return failed(-1);
  }
}

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

TEST(ResultTest, SimpleTestCase) {
  const auto r = foo(1, 2);
  ASSERT_TRUE(r.is_ok()) << "1 < 2 so we should have ok here";
  ASSERT_EQ(r.unwrap(), 3) << "we should have result of 1 + 2 but we have " << r;

  // now test the case where we would fail
  const auto e = foo(2, 1);
  ASSERT_TRUE(e.is_error()) << "this should fail as 2 > 1";
  ASSERT_EQ(e.error_value(), 1) << "the value now should be 2 - 1 but we have " << e.error_value();

  const auto e2 = bar("this is some string", ' ');  // should failed, we have space here
  ASSERT_TRUE(e2.is_error()) << "this should fail as we have space in the string";
  ASSERT_EQ(e2.error_value(), -1) << "the value now should be -1 but we have: " << e.error_value();

}

TEST(ResultTest, WithEnum) {
  const auto r = f(good_result::BEST), r2 = f(good_result::EVEN_BETTER), r3 = f(good_result::OK);
  ASSERT_EQ(r.is_error(), r2.is_error()) << "both r and r2 should failed";
  ASSERT_NE(r.is_error(), r3.is_error()) << "r3 should succeed and r should failed so they are not equal";
  ASSERT_EQ(r.error_value(), 1);
  ASSERT_EQ(r2.error_value(), -1);
  ASSERT_EQ(r3.unwrap(), good_result::OK) << "we have have ok in this case inside the return ok value";
}

TEST(ResultTest, WithUDStruct) {
  const std::string_view error_inputs[] {
    {}, // error
    "1, 2, 3, 4, 5, 6, 7, 8, 9, 10"
  };
  for (auto&& s : error_inputs) {
    const auto r = f2(s);
    ASSERT_TRUE(r.is_error()) << " this should failed for all these cases: " << s << ", got " << r;
  }
  ASSERT_TRUE(f2("good").is_ok()) << "we are expecting that if the string is not empty and not long its good";
}