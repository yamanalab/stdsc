cmake_minimum_required(VERSION 2.8)

set(project_name stdsc)
project(${project_name})

find_package(Threads REQUIRED)
set(CMAKE_CXX_FLAGS "-O3 -std=c++11 -pthread -Wall -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -g -std=c++11 -pthread -Wall")

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(stdsc)
add_subdirectory(examples)
