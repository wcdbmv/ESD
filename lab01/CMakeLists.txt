cmake_minimum_required(VERSION 3.25)
project(lab01)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
	HEADERS
	graph/breadth_first_search.hpp
	graph/depth_first_search.hpp
	graph/graph.hpp
	graph/pattern_search_result.hpp
	graph/search_params.hpp
	graph/search_result.hpp
	types/common.hpp
	types/graph.hpp
	utils/convert.hpp
	utils/io_utils.hpp
)

set(
	SOURCES
	graph/breadth_first_search.cpp
	graph/depth_first_search.cpp
	graph/graph.cpp
	graph/search_params.cpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)
