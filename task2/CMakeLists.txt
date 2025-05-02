cmake_minimum_required(VERSION 3.10)
project(TaskServerApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main_exec main.cpp)

add_executable(test_exec test_results.cpp)
