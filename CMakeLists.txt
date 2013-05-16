cmake_minimum_required(VERSION 2.8.3)
project(bp3)

add_subdirectory(cmake)

set(LIBRARY_TYPE SHARED)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PYTHON_INCLUDE_DIRS}
)

add_subdirectory(lib)
