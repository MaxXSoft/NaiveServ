cmake_minimum_required(VERSION 2.8)
project(pcaptest)

# # set compiler path
# set(CMAKE_C_COMPILER "/usr/local/opt/gcc/bin/gcc-8")
# set(CMAKE_CXX_COMPILER "/usr/local/opt/gcc/bin/g++-8")

# C++17 standard support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# enable all warnings and treat them as errors
if(MSVC)
  add_compile_options(/W3 /WX)
else()
  add_compile_options(-Wall -Werror)
endif()

# project include directories
include_directories(src)

# all of C++ source files
# file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE SOCK_TEST_SRC "test/sock_test.cpp" "src/sock/*.cpp")
file(GLOB_RECURSE RESPONSE_TEST_SRC "test/response_test.cpp" "src/http/response.cpp")

# executable
# add_executable(naiveserv ${SOURCES})
add_executable(sock_test ${SOCK_TEST_SRC})
add_executable(response_test ${RESPONSE_TEST_SRC})
