add_library(rlImGui STATIC "${CMAKE_CURRENT_SOURCE_DIR}/external/rlImGui/rlImGui.cpp")
target_include_directories(rlImGui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/rlImGui" "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
target_link_libraries(rlImGui PUBLIC raylib imgui)

set(rlImgui_FOUND true)
