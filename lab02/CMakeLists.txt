cmake_minimum_required(VERSION 3.25)
project(lab02)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
  HEADERS
  graph/breadth_first_search.h
  graph/graph.h
  types/common.h
  types/graph.h
  utils/io_utils.h
)

set(
  SOURCES
  graph/breadth_first_search.cc
  graph/graph.cc
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cc)
