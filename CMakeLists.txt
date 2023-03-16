cmake_minimum_required(VERSION 3.16)
project(bplus_tree)

set(CMAKE_CXX_STANDARD 17)

add_executable(bplus_tree main.cpp node.h bplustree.h)
