cmake_minimum_required(VERSION 4.3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_PREFIX_PATH cmake)

project(CrocobyGraph LANGUAGES C CXX VERSION 0.1.0)

# Library

find_package(raylib REQUIRED)
find_package(imgui REQUIRED)
find_package(rlImgui REQUIRED)
find_package(entt REQUIRED)

file(GLOB CGRAPH_SRC "src/*.cpp")
add_library(CrocobyGraph ${CGRAPH_SRC})

target_include_directories(CrocobyGraph PUBLIC ./include)
target_link_libraries(CrocobyGraph PUBLIC rlImGui entt)

# Demo executable

add_executable(DemoCrocobyGraph ./bin/main.cpp)
target_link_libraries(DemoCrocobyGraph PRIVATE CrocobyGraph)
