cmake_minimum_required(VERSION 4.2)
project(pipsh C)

set(CMAKE_C_STANDARD 17)

add_executable(pipsh src/main.c src/parser.c src/builtins.c src/prefix.c)

target_include_directories(pipsh PRIVATE include)
