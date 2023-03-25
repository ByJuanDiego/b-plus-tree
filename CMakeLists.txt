cmake_minimum_required(VERSION 3.16)
project(b_plus_tree)

include_directories(utils/include src include)
set(CMAKE_CXX_STANDARD 20)

add_executable(b_plus_tree src/main.cpp include/node.hpp include/bplustree.hpp src/bplustree.cpp src/node.cpp utils/include/transaction.hpp utils/src/transaction.cpp)
