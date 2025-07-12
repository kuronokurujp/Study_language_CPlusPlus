cmake_minimum_required(VERSION 3.14)
project(googletest NONE)

add_library(gtest INTERFACE)
target_include_directories(gtest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/googletest/include)

add_library(gtest_main STATIC googletest/src/gtest_main.cc)
target_link_libraries(gtest_main PUBLIC gtest)
