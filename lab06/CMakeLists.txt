cmake_minimum_required(VERSION 3.25)
project(lab06)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
  HEADERS
  types/atom.h
  types/clause.h
  types/formula.h
  types/terminal.h
)

set(
  SOURCES
  types/atom.cc
  types/clause.cc
  types/formula.cc
  types/terminal.cc
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cc)
