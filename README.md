# result
this is a simple Rust like result to be used in function calls

## Overview
In many function programing langauges you have the monadic type that is used as a result from a function.
A function can either run a good result, or failed with some posiblly other type.
This idom is used in the [Rust programing langauge](https://www.rust-lang.org/). 
In Rust we have [std::result](https://doc.rust-lang.org/std/result/). This algebric type is used in Rust for basically every function that can fail.
Unlike C++ Rust don't have support for exceptions.
The thing about exceptions is that they are expensive to use when they fail, and they should be used only for those cases where we are not expecting to continue.
Unlike exception, result allow the programmer to take action in case of a failure, and they are cheap.
C++ 23 will add support for this idom [the std::expected](https://en.cppreference.com/w/cpp/utility/expected). This is not fully competible with Rust as C++ lack the pattern matching that is used with Result and it also lacks that "?" to short circte in case of a failure.

## Usage
In this case I tries to make the code as simple as possible and use as much C++ exiting support without re-inventing the wheel. 
The requirement is for C++ 17 support in your compiler (though this is required only on a single place in the code.

A simple use case would look like:
```cpp
#include "result.h"
#include <iostream>
#include <cstdlib>

auto foo(int a, int b) -> result<int. int> {
  if (a < b) {
    return ok{a + b};
  } else {
    return failed(a - b};
  }
}

auto main(int argc, char** argv) -> int {
  if (argc > 2 && ((argc - 1) % 2 == 0)) {
    for (int i = 1; i < argc; i++) {
      auto a = std::atoi(argv[i]);
      auto b = std::atoi(argv[i + 1]);
      const auto r = foo(a, b);
      if (r) {
        std::cout << "the good result " << a << " and " << b <<", are " << r.unwrap() << "\n";
      } else {
        std::cout <<" failed with result " << r << " for " << a << " and " << b << "\n";
      }
    }
  } else {
    std::cerr<< "usage: <num 1> <num2> ..\n";
    return -1;
}
```
Note that unlike normal return value from a function, you cannot ignore the result, and you must check whether you were successful or not.
