cmake_minimum_required(VERSION 3.13)
SET(CMAKE_USE_RELATIVE_PATHS TRUE)
add_subdirectory(bin)
project(compiler C)
file(GLOB_RECURSE SRC_DIR
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.h)
add_executable(compiler ${SRC_DIR})