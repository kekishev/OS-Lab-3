cmake_minimum_required(VERSION 4.0)
project(DFT C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

function(flags target)
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic)
endfunction()

add_library(Bigint SHARED src/main/bigint/bigint.c)
target_include_directories(Bigint PUBLIC src/main/bigint)
flags(Bigint)

add_executable(lab src/main/bigint/lab/lab.c)
target_link_libraries(lab PRIVATE Bigint)
