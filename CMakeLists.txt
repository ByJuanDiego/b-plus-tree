cmake_minimum_required(VERSION 3.16)
project(b_plus_tree)

set(CMAKE_CXX_STANDARD 20)

add_executable(b_plus_tree main.cpp include/node.hpp include/bplustree.hpp src/bplustree.cpp src/node.cpp utils/include/transaction.hpp utils/src/transaction.cpp)
