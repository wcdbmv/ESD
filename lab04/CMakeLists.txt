cmake_minimum_required(VERSION 3.25)
project(lab04)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(
  HEADERS
  atom.h
  clause.h
  io_join.h
  resolution.h
  resolvent.h
)

set(
  SOURCES
  atom.cc
  clause.cc
  resolution.cc
  resolvent.cc
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cc)
