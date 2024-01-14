cmake_minimum_required(VERSION 3.25)
project(lab02)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
  HEADERS
  graph/breadth_first_search.hpp
  graph/graph.hpp
  types/common.hpp
  types/graph.hpp
  utils/io_utils.hpp
)

set(
  SOURCES
  graph/breadth_first_search.cpp
  graph/graph.cpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)
