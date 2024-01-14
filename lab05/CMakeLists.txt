cmake_minimum_required(VERSION 3.25)
project(lab05)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
	HEADERS
	types/atom.hpp
	types/constant.hpp
	types/name.hpp
	types/terminal.hpp
	types/variable.hpp
	unification.hpp
)

set(
	SOURCES
	types/atom.cpp
	types/terminal.cpp
	unification.cpp
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)
