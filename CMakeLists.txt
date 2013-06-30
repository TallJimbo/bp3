cmake_minimum_required(VERSION 2.8.3)
project(bp3)

enable_testing()

add_subdirectory(cmake)

set(LIBRARY_TYPE SHARED)
set(CMAKE_BUILD_TYPE Debug)

include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${PYTHON_INCLUDE_DIRS}
)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/include/bp3/builtin/exceptions.hpp
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/include/bp3/builtin/exceptions.hpp.py ${CMAKE_BINARY_DIR}/include/bp3/builtin/exceptions.hpp
  DEPENDS ${PROJECT_SOURCE_DIR}/buildutils.py ${CMAKE_SOURCE_DIR}/include/bp3/builtin/exceptions.hpp.py
)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/builtin/exceptions.cpp
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/builtin/exceptions.cpp.py ${CMAKE_BINARY_DIR}/src/builtin/exceptions.cpp
  DEPENDS ${PROJECT_SOURCE_DIR}/buildutils.py ${CMAKE_SOURCE_DIR}/src/builtin/exceptions.cpp.py
)

add_library(bp3 ${LIBRARY_TYPE}
  # header files
  include/bp3/utils/Any.hpp
  include/bp3/callables/Overload.hpp
  include/bp3/callables/Callable.hpp
  include/bp3/callables/ArgsFromPython.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/include/bp3/builtin/exceptions.hpp
  include/bp3/builtin/object.hpp
  include/bp3/builtin/numbers.hpp
  include/bp3/builtin/str.hpp
  include/bp3/builtin/sequence.hpp
  include/bp3/TypeInfo.hpp
  include/bp3/FromPython.hpp
  include/bp3/FromPythonBase.hpp
  include/bp3/FromPythonFuncs.hpp
  include/bp3/Registration.hpp
  include/bp3/builtin.hpp
  include/bp3/PyPtr.hpp
  include/bp3/Module.hpp
  # source files
  src/callables/Overload.cpp
  src/callables/Callable.cpp
  src/Module.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/src/builtin/exceptions.cpp
  src/builtin/object.cpp
  src/builtin/numbers.cpp
  src/builtin/str.cpp
  src/builtin/sequence.cpp
  src/FromPythonBase.cpp
  src/Registration.cpp
  src/TypeInfo.cpp
)

add_subdirectory(python/bp3)
add_subdirectory(tests)
