cmake_minimum_required(VERSION 2.8)
project(parallel_variance)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

link_directories(/usr/lib/x86_64-linux-gnu)

add_library(spmc SpMeanCovariance.cpp)

add_executable(meancov_test SpMeanCovariance_test.cc)
target_link_libraries(meancov_test spmc gtest gtest_main pthread)

