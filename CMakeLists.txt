cmake_minimum_required(VERSION 4.3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

project(CrocobyGraph LANGUAGES C CXX VERSION 0.1.0)

# Library

file(GLOB CGRAPH_SRC "src/*.cpp")
add_library(CrocobyGraph ${CGRAPH_SRC})

target_include_directories(CrocobyGraph PUBLIC ./include)

# Demo executable

add_executable(DemoCrocobyGraph ./bin/main.cpp)
target_link_libraries(DemoCrocobyGraph PRIVATE CrocobyGraph)
