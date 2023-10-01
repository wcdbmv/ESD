cmake_minimum_required(VERSION 3.23)
project(lab03)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
	HEADERS
	graph/depth_first_search.hpp
	graph/graph.hpp
	types/common.hpp
	types/graph.hpp
	utils/io_utils.hpp
)

set(
	SOURCES
	graph/depth_first_search.cpp
	graph/graph.cpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(${PROJECT_NAME} INTERFACE
	"$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Werror;-Wall;-Wextra;-Wpedantic-Wcast-align;-Wcast-qual;-Wconversion;-Wctor-dtor-privacy;-Wenum-compare;-Wextra-semi;-Wfloat-equal;-Wnon-virtual-dtor;-Wold-style-cast;-Woverloaded-virtual;-Wredundant-decls;-Wshadow;-Wsign-conversion;-Wsign-promo;-Wzero-as-null-pointer-constant>>"
	"$<${msvc_cxx}:$<BUILD_INTERFACE:-W4>>"
)
