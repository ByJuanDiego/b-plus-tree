cmake_minimum_required(VERSION 3.16)
project(bplus_tree)

set(CMAKE_CXX_STANDARD 17)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/transactions.txt ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(bplus_tree main.cpp node.h bplustree.h)
