cmake_minimum_required(VERSION 2.8.3)

python_add_module(from_python_test_mod from_python.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/from_python_test.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(from_python ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/from_python_test.py")

target_link_libraries(from_python_test_mod bp3)
