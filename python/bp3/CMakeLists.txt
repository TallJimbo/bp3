cmake_minimum_required(VERSION 2.8.3)

python_add_module(parser parser.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(parser bp3)
