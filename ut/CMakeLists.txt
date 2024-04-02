# we are enabling as much warning as possible here so that we will be able to detect
# issues at comile time
include(dependencies)
include(flags)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
enable_testing()
list(APPEND EXTRA_INCLUDES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>)
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
add_executable(
  monadic_op
  monadic_op.cpp
)

target_link_libraries(
  monadic_op
  GTest::Main
)
gtest_discover_tests(monadic_op)

target_include_directories(monadic_op PUBLIC ${EXTRA_INCLUDES})
target_include_directories(basic_usage PUBLIC ${EXTRA_INCLUDES})
