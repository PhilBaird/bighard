cmake_minimum_required(VERSION 3.17)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(atm atm.c)
add_executable(dbserver dbserver.c sema.c sema.h db.txt)
add_executable(dbeditor dbeditor.c)