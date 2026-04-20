cmake_minimum_required(VERSION 3.30.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(CrocobyGraph LANGUAGES C CXX VERSION 0.1.0)

# Library

find_package(raylib REQUIRED)
find_package(imgui REQUIRED)
find_package(rlImgui REQUIRED)
find_package(entt REQUIRED)

add_library(CrocobyGraph
  ./src/resources/editor_icons.cpp
  ./src/resources/open_sans.cpp
  ./src/batch.cpp
  ./src/color.cpp
  ./src/decompose.cpp
  ./src/ecs.cpp
  ./src/editor_frame.cpp
  ./src/math.cpp
  ./src/painter.cpp
  ./src/physics.cpp
  ./src/physics_frame.cpp
  ./src/physics_system.cpp
  ./src/scene.cpp
  ./src/window.cpp
  ./src/window_system.cpp
)

target_include_directories(CrocobyGraph PUBLIC ./include)
target_link_libraries(CrocobyGraph PUBLIC rlImGui entt)

# Demo executable

add_executable(DemoCrocobyGraph ./bin/main.cpp)
target_link_libraries(DemoCrocobyGraph PRIVATE CrocobyGraph)
