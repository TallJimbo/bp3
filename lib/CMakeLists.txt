cmake_minimum_required(VERSION 2.8.3)

add_library(bp3 ${LIBRARY_TYPE}
  # header files
  ../include/bp3/testutils.hpp
  ../include/bp3/utils/any.hpp
  ../include/bp3/keywords.hpp
  ../include/bp3/callable.hpp
  ../include/bp3/builtin/exceptions.hpp
  ../include/bp3/builtin/object.hpp
  ../include/bp3/builtin/numbers.hpp
  ../include/bp3/builtin/str.hpp
  ../include/bp3/type_id.hpp
  ../include/bp3/conversion/from_python.hpp
  ../include/bp3/conversion/args_from_python.hpp
  ../include/bp3/conversion/registration.hpp
  ../include/bp3/conversion/from_python_base.hpp
  ../include/bp3/conversion/from_python_funcs.hpp
  ../include/bp3/builtin.hpp
  ../include/bp3/meta.hpp
  ../include/bp3/py_ptr.hpp
  ../include/bp3/context.hpp
  # source files
  ../src/callable.cpp
  ../src/context.cpp
  ../src/builtin/object.cpp
  ../src/builtin/numbers.cpp
  ../src/builtin/exceptions.cpp
  ../src/builtin/str.cpp
  ../src/conversion/from_python_base.cpp
  ../src/conversion/registration.cpp
  ../src/keywords.cpp
)
