cmake_minimum_required(VERSION 2.8.3)

python_add_module(from_python_mod from_python.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/from_python.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_test(from_python ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/from_python.py")

python_add_module(callables_mod callables.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/callables.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_test(callables ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/callables.py")

target_link_libraries(from_python_mod bp3)
target_link_libraries(callables_mod bp3)
