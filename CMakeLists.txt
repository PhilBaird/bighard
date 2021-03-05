# cmake_minimum_required(VERSION <specify CMake version here>)
project(bighard C)

set(CMAKE_C_STANDARD 11)
set(SOURCE_FILES test.txt)

add_executable(bighard
        kernel.c ${SOURCE_FILES})
