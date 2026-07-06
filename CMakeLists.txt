cmake_minimum_required(VERSION 4.0)
project(DFT C)

set(CMAKE_C_STANDARD 11)

add_executable(
        DFT src/main/main.c
        src/main/bigint/bigint.h
        src/main/dft/dft.h
)
