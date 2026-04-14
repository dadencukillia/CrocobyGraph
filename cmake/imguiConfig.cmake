file(GLOB IMGUI_SOURCES "${CMAKE_SOURCE_DIR}/external/imgui/*.cpp")
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC "${CMAKE_SOURCE_DIR}/external/imgui")

set(imgui_FOUND true)
