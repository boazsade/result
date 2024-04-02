# result
this is a simple Rust like result to be used in function calls.

# Cloning
This depends on git submodule,  so you to clone with submodules as well:
```bash
git clone --recurse-submodules https://github.com/boazsade/result.git
```
or if you're closing with ssh:
```bash
git clone --recurse-submodules git@github.com:boazsade/result.git
```

## Overview
In many function programming languages you have the monadic type that is used as a result from a function.
A function can either run a good result, or failed with some possibly other type.
This idiom is used in the [Rust programming language](https://www.rust-lang.org/). 
In Rust we have [std::result](https://doc.rust-lang.org/std/result/). This algebraic type is used in Rust for basically every function that can fail.
Unlike C++ Rust don't have support for exceptions.
The thing about exceptions is that they are expensive to use when they fail, and they should be used only for those cases where we are not expecting to continue.
Unlike exception, result allow the programmer to take action in case of a failure, and they are cheap.
C++ 23 will add support for this idom [the std::expected](https://en.cppreference.com/w/cpp/utility/expected). This is not fully compatible with Rust as C++ lack the pattern matching that is used with Result and it also lacks that "?" to short circuit in case of a failure.

### Some notes regarding error handling
The term "error" is a little board in regard to programming.
I general there can be at least two categories:
1. Critical error that we cannot recover from. These error can only lead to graceful restart of the current state.
2. Failure of execution. This is the case where a function was called, but it cannot perform it task.
#### Critical Errors:
An example for this case is where we are trying to allocate memory to the application from the free heap, and the operation fails.
This is a none recoverable state, and any attempt to continue from this point, will only worsen the state at which we are right now.
These family of errors is best covered by C++ exceptions. I tried to do something and I came across an unrecoverable state, where the only sane solution is to unwind the stack and leave.
This is indeed an error and is rightfully called exception, because this is indeed some exceptional case that is not part of the normal flow of the application.
> Note that this "result" type is not meant in any way to replace it.
#### Results
When a function is run, it should be able to report correctly what was the result of it execution. These results must not be ignored and they also must be different when the function was successful or not.
For example the function `atoi` in the function programming language will always return an `int`, but whether this function was successful or not cannot be determine from its result. Any integer value is a "good" value.
On the other hand we would not like to make the call not agronomical (for example not returning an `int` in case of success), and in case where the argument to the function cannot be interrupt as `int` there is no real reason to throw an exception.
In this case we can use the `result` type. We can return an `int` in case we successfully parsed the all string as `int` or return a failure in case this is not really an `int`, but with detail. In this case, the caller will know what went wrong, and either handle only the case where the translation was successful, or when it failed or produce a default value. for example:
```cpp
if (const auto as_int = std::atoi(some_string); as_int) {
  // we only care if success, ignore the error case
}

if (const auto as_int_fail = Err(std::atoi(some_string))) {
  // handle the case where this is not an int
}

const auto my_int = std::atoi(some_sting).or_else([](const auto&) -> <int, int>  {
  return 0; // produce 0 in case we failed
});
```

## Usage
In this case I tries to make the code as simple as possible and use as much C++ exiting support without re-inventing the wheel. 
The requirement is for C++ 17 support in your compiler (though this is required only on a single place in the code).
> Please note that this is now set to use C++20 but there is no real requirement for it in the code itself.

A simple use case would look like:
```cpp
#include "result.h"
#include <iostream>
#include <cstdlib>

auto foo(int a, int b) -> result<int. int> {
  if (a < b) {
    return ok{a + b};
  } else {
    return failed{a - b};
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
}
```
Note that unlike normal return value from a function, you cannot ignore the result, and you must check whether you were successful or not.

### Monadic Operations
Just like in Rust, you can use monadic options to chain operations based on the results.
You have these operations:
- and_then: This will run a function if the last result is successfully. Note that the result of this function should be of type "result" and the the fail case must match the original function.
- or_else: This will run a function if the last result is failure. Note that the result of this function should be of type "result" and the the success case must match the original function.
- unwrap_or: This will try to extract the success value or return some default value that you are passing to the function.
- map_err: this will return some new result, where the success type is the same, but the error is mapped to new type based on some function.
For example:
```cpp
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

auto main() -> int {
  const int b = 10, a = 20;
    const int factor = 5;
    const auto r = foo(a, b).or_else([factor](auto e) -> result<int, int> {
        return ok(e * factor);
    });

    const auto r = bar("some string", ' ').or_else([](auto&& e) -> result<std::string, error_case> {
        // the error type here is int, lets convert this to enum
        return failed(error_case("I didn't like what I see here: " + std::to_string(e)));
    });

    const std::string expected_value = "expected";

    const auto r = bar("this is a string", ' ').unwrap_or(expected_value);
    const auto r = bar("some string", '+').and_then([](auto&& e) -> result<std::string, int> {
        // the error type here is int, lets convert this to enum
        return failed(int(e.size()));
    });
}
```
Note that for lambda function, you may need to explicitly set the return type since this sometime cannot be deduce from the lambda itself.
Note that even in Rust this is required, so I don't see this as a reason to change.

## Build
This project contain cmake in order to build its tests.
The actual code is not require any build since it is header only.
Please note that in order to build locally you would need to have conan support on install the dependencies yourself.
> In general the only real requirement to run the tests is to have [google's gtest](https://github.com/google/googletest).
You can manually set this with the commands:
```bash
conan install .  --output-folder=build --build=missing
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
	    -DCMAKE_BUILD_TYPE=<Debug|Release> \
	    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
	    -DCMAKE_POLICY_DEFAULT_CMP0091=NEW \
	    -G Ninja ..
cmake --build .
ctest --progress -VV
```
Or if you're running on a Linux like environment you can run it using the scripts:
* build.sh -> this will build with a list of arguments (run ./build.sh --help for the full details on the available options).
* build-debug.sh -> will build a debug version using the above script.
* build-release.sh -> build a release version using build.sh
* If you Docker installed you can using the scripts 'docker_setup.sh' 'init_build.sh' to run the builds inside the container that has all the dependencies ready.

## Tests and Examples
The examples section here will contain more examples on how to use this. Take a look there.
To run the unit test:
```sh
docker build -t test-image -f dev-container/Dockerfile ,
```
And then
```sh
export GID=$(id -g)
docker run --rm -t -d --user $UID:$GID \
    --workdir="/home/$USER" --volume="/etc/group:/etc/group:ro" \
    --name test-image -v <path to this repo>:/builds
    --volume="/etc/passwd:/etc/passwd:ro" \
    --volume="/etc/shadow:/etc/shadow:ro" \
    test-image
```
Then to build using the above image:
```sh
docker exec test-image /builds/dev-container/build.sh -b /builds/releases -e 1 -g 1 -u 1
```
## Build
You can build the unit tests (since the usage of this is a single H file that don't need to be built), using cmake.
Note that you have some help files that can help you build automatically (all of them are based on the file "build.sh").
You can also run the builds and tests with the dockerfile that is located here.
Using the build scripts for debug:
```bash
dev-container/build.sh -g 1 -t Debug -b build-debug
dev-container/build.sh -t Debug -b build-debug
```
For release
```bash
dev-container/build.sh -g 1 -t Release -b build-release
dev-container/build.sh -t Release -b build-release
```
Or you can manually run the cmake commands to generate the builds.
Note that this since this is an header only implementation you don't really need to build it.