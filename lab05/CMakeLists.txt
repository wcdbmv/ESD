cmake_minimum_required(VERSION 3.25)
project(lab05)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
  HEADERS
  types/atom.h
  types/constant.h
  types/name.h
  types/terminal.h
  types/variable.h
  unification.h
)

set(
  SOURCES
  types/atom.cc
  types/terminal.cc
  unification.cc
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cc)
