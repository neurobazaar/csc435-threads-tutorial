cmake_minimum_required(VERSION 3.22)

project(run-tasks
        VERSION 1.0
        DESCRIPTION "C++ Multithreading Example"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(run-tasks src/run-tasks.cpp)

target_include_directories(run-tasks PUBLIC include)