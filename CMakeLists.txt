cmake_minimum_required(VERSION 3.25)
project(ESD
        HOMEPAGE_URL https://github.com/wcdbmv/ESD
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/CompileOptions.cmake)

set(CMAKE_CXX_CLANG_TIDY clang-tidy -warnings-as-errors=*)

add_subdirectory(lab01)
add_subdirectory(lab02)
add_subdirectory(lab03)
add_subdirectory(lab04)
add_subdirectory(lab05)
add_subdirectory(lab06)
