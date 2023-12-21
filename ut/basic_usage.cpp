#include "result.h"
#include <gtest/gtest.h>
#include <string>

namespace
{

auto foo(int a, int b) -> result<int, int> {
  if (a < b) {
    return ok{a + b};
  } else {
    return failed{a - b};
  }
}

}  // end of local namespace

TEST(ResultTest, SimpleTestCase) {
  const auto r = foo(1, 2);
  ASSERT_TRUE(r.is_ok()) << "1 < 2 so we should have ok here";
  ASSERT_EQ(r.unwrap(), 3) << "we should have result of 1 + 2 but we have " << r;

  // now test the case where we would fail
  const auto e - foo(2, 1);
  ASSERT_TRUE(e.is_error()) << "this should fail as 2 > 1";
  ASSERT_EQ(e.error_value(), 1) << "the value now should be 2 - 1 but we have " << e.error_value();
}
