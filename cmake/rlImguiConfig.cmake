add_library(rlImGui STATIC "${CMAKE_SOURCE_DIR}/external/rlImGui/rlImGui.cpp")
target_include_directories(rlImGui PUBLIC "${CMAKE_SOURCE_DIR}/external/rlImGui" "${CMAKE_SOURCE_DIR}/external/imgui")
target_link_libraries(rlImGui PRIVATE raylib imgui)

set(rlImgui_FOUND true)
