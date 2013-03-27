cmake_minimum_required(VERSION 2.8.3)
project(bp3)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(CheckCXX11Features)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")
set(LIBRARY_TYPE SHARED)

find_package(PythonInterp REQUIRED)
find_package(PythonLibsNew REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PYTHON_INCLUDE_DIRS}
)

add_library(bp3 ${LIBRARY_TYPE}
  # header files
  include/bp3/testutils.hpp
  include/bp3/utils/any.hpp
  include/bp3/keywords.hpp
  include/bp3/callable.hpp
  include/bp3/builtin/exceptions.hpp
  include/bp3/builtin/object.hpp
  include/bp3/builtin/numbers.hpp
  include/bp3/builtin/str.hpp
  include/bp3/type_id.hpp
  include/bp3/conversion/from_python.hpp
  include/bp3/conversion/args_from_python.hpp
  include/bp3/conversion/registration.hpp
  include/bp3/conversion/from_python_base.hpp
  include/bp3/conversion/from_python_funcs.hpp
  include/bp3/builtin.hpp
  include/bp3/meta.hpp
  include/bp3/py_ptr.hpp
  include/bp3/context.hpp
  # source files
  src/callable.cpp
  src/context.cpp
  src/builtin/object.cpp
  src/builtin/numbers.cpp
  src/builtin/exceptions.cpp
  src/builtin/str.cpp
  src/conversion/from_python_base.cpp
  src/conversion/registration.cpp
  src/keywords.cpp
)
