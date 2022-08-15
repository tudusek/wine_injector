cmake_minimum_required(VERSION 3.19)
project(wine_injector)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)

add_executable(wine_injector main.c)
target_link_libraries(wine_injector PUBLIC "-static -s")
