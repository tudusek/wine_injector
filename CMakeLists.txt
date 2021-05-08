cmake_minimum_required(VERSION 3.19)
project(wine_injector)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

add_executable(wine_injector main.cpp)
target_link_libraries(wine_injector PUBLIC "-static -s")