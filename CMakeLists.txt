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
  ./src/resources/media_controls_icons.cpp
  ./src/resources/circle_icons.cpp

  ./src/interface/batch.cpp
  ./src/interface/color.cpp
  ./src/interface/decompose.cpp
  ./src/interface/ecs.cpp
  ./src/interface/editor_frame.cpp
  ./src/interface/physics_frame.cpp
  ./src/interface/physics_system.cpp
  ./src/interface/scene.cpp
  ./src/interface/window_system.cpp
  ./src/interface/thread_pool.cpp
  ./src/interface/thread_pool_counter.cpp
  ./src/interface/bfs_frame.cpp
  ./src/interface/grid.cpp
  ./src/interface/adjacency_matrix.cpp
  ./src/interface/dfs_frame.cpp

  ./src/internal/window.cpp
  ./src/internal/math.cpp
  ./src/internal/painter.cpp
  ./src/internal/physics.cpp
  ./src/internal/calc_impls.cpp
  ./src/internal/resources.cpp
  ./src/internal/labels.cpp
)

target_include_directories(CrocobyGraph PUBLIC ./include)
target_link_libraries(CrocobyGraph PUBLIC rlImGui entt)

# Demo executable

add_executable(DemoCrocobyGraph ./bin/main.cpp)
target_link_libraries(DemoCrocobyGraph PRIVATE CrocobyGraph)
