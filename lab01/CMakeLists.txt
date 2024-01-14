cmake_minimum_required(VERSION 3.25)
project(lab01)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
  HEADERS
  graph/breadth_first_search.h
  graph/depth_first_search.h
  graph/graph.h
  graph/pattern_search_result.h
  graph/search_params.h
  graph/search_result.h
  types/common.h
  types/graph.h
  utils/convert.h
  utils/io_utils.h
)

set(
  SOURCES
  graph/breadth_first_search.cc
  graph/depth_first_search.cc
  graph/graph.cc
  graph/search_params.cc
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cc)
