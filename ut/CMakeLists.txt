# we are enabling as much warning as possible here so that we will be able to detect
# issues at comile time
include(dependencies)
include(flags)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
enable_testing()
find_package(GTest REQUIRED)
add_executable(
  basic_usage
  basic_usage.cpp
)
target_link_libraries(
  basic_usage
  GTest::Main
)


gtest_discover_tests(basic_usage)
