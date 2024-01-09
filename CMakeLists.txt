cmake_minimum_required(VERSION 3.27)
project(enigma C)

set(CMAKE_C_STANDARD 11)

add_library(enigma SHARED src/enigma.c)
include_directories(src)

add_executable(test tests/test.c)
target_link_libraries(test enigma)