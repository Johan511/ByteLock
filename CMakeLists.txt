cmake_minimum_required (VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(range_lock)

add_library(range_lock STATIC src/lock2.cpp)

target_include_directories(range_lock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)
