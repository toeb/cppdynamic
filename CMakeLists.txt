cmake_minimum_required(VERSION 2.8.7)
project(core_dynamic)

file(GLOB_RECURSE files include/**.h)
message("files ${files}")
include_directories(include)
add_executable(test test.cpp ${files})


