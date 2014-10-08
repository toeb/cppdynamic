cmake_minimum_required(VERSION 2.8.7)
project(core_dynamic)
if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-std=c++0x)
endif()
file(GLOB_RECURSE files include/**.h)
message("files ${files}")
include_directories(include)
add_executable(test test.cpp ${files})


