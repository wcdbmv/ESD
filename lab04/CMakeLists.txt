cmake_minimum_required(VERSION 3.25)
project(lab04)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
  HEADERS
  atom.hpp
  clause.hpp
  io_join.hpp
  resolution.hpp
  resolvent.hpp
)

set(
  SOURCES
  atom.cpp
  clause.cpp
  resolution.cpp
  resolvent.cpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)
