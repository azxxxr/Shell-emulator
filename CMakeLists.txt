cmake_minimum_required(VERSION 3.10)
project(shell_emulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_executable(shell-emulator
    src/main.cpp
    src/parser.hpp
    src/shell.hpp
    src/commands.hpp
)
