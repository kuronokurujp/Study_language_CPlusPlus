cmake_minimum_required(VERSION 3.10)
project(googletest)

add_library(gtest INTERFACE)
target_include_directories(gtest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(gtest_main STATIC src/gtest_main.cc)
target_link_libraries(gtest_main PUBLIC gtest)

add_library(GTest::gtest ALIAS gtest)
add_library(GTest::gtest_main ALIAS gtest_main)
