project(nskicpp-examples)
cmake_minimum_required(VERSION 3.1)

# GCC and Clang both default to using an old C++ standard, so we need
# to explicitly tell them to use the version we're using, C++14.  This
# will make all targets use C++14, which is what we want here.
set(CMAKE_CXX_STANDARD          14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hello1 hello1.cpp)
add_executable(hello2 hello2.cpp)

enable_testing()
add_test(NAME test_hello1 COMMAND hello1)
add_test(NAME test_hello2 COMMAND hello1)
set_tests_properties(test_hello1 test_hello2
  PROPERTIES PASS_REGULAR_EXPRESSION "Hello, World!")