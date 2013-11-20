cmake_minimum_required(VERSION 2.8.3)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

python_add_module(_bp3 _bp3.cpp)
target_link_libraries(_bp3 bp3)