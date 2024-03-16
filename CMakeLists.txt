cmake_minimum_required(VERSION 3.13)
add_subdirectory(bin)
project(compiler C)
file(GLOB_RECURSE SRC_DIR
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.h)
add_executable(compiler ${SRC_DIR})

enable_testing()
find_program (BASH_PROGRAM bash)
add_test(NAME run_test COMMAND "bash" ../test.sh)