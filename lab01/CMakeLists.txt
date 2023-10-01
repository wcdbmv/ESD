cmake_minimum_required(VERSION 3.23)
project(lab01)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(${PROJECT_NAME} INTERFACE
	"$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Werror;-Wall;-Wextra;-Wpedantic-Wcast-align;-Wcast-qual;-Wconversion;-Wctor-dtor-privacy;-Wenum-compare;-Wextra-semi;-Wfloat-equal;-Wnon-virtual-dtor;-Wold-style-cast;-Woverloaded-virtual;-Wredundant-decls;-Wshadow;-Wsign-conversion;-Wsign-promo;-Wzero-as-null-pointer-constant>>"
	"$<${msvc_cxx}:$<BUILD_INTERFACE:-W4>>"
)
