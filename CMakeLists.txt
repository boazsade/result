cmake_minimum_required(VERSION 3.0.0)
# we are enabling as much warning as possible here so that we will be able to detect
# issues at comile time
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  # https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Wextra -Wformat -Wformat-security")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-overflow -Wnull-dereference -Wnonnull")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverlength-strings")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-declaration-mismatch")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op -Wfloat-conversion -Wdangling-else -Wconversion -Wcast-align -Wcast-qual -Wshadow")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal -Wstringop-overflow")
endif()
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
enable_testing()
find_package(GTest REQUIRED)
add_executable(
  unittest_result
  test_ut.cpp
)
target_link_libraries(
  unittest_result
  GTest::Main
)


gtest_discover_tests(unittest_result)
