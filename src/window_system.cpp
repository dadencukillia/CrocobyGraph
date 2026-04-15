#include "window_system.hpp"
#include "ecs.hpp"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include <cmath>
#include <cstdint>

namespace CrocobyGraph {

  System<GraphECS> get_window_system(bool debug_gui) {
    return {
      .init_callback = [debug_gui](auto ev) {
        InitWindow(1200, 800, "Graph View");
        SetWindowState(FLAG_WINDOW_RESIZABLE);
        if (debug_gui) {
          rlImGuiSetup(true);
          ImGui::GetIO().IniFilename = nullptr;
        }
      },
      .tick_callback = [debug_gui](auto ev) {
        if (WindowShouldClose()) {
          ev.ecs->clear_systems();
        }

        BeginDrawing();
      
        ClearBackground({ 36, 35, 44, 255 });
        DrawText("Hello", 0, 0,  16, ::BLACK);

        for (int32_t x = 0; x <= GetScreenWidth(); x += 30) {
          for (int32_t y = 0; y <= GetScreenHeight(); y += 30) {
            DrawRectangle(x, y, 2, 2, { 54, 53, 61, 255 });
          }
        }

        if (debug_gui) {
          rlImGuiBegin();

          ImGui::Begin("Graph Editor");

          float samples[100];
          for (int n = 0; n < 100; n++)
              samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
          ImGui::PlotLines("Samples", samples, 100);

          ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
          ImGui::BeginChild("Scrolling");
          for (int n = 0; n < 50; n++)
              ImGui::Text("%04d: Some text", n);
          ImGui::EndChild();

          ImGui::End();

          rlImGuiEnd();
        }

        EndDrawing();
      },
      .remove_callback = [debug_gui](auto ev) {
        if (debug_gui) rlImGuiShutdown();
        CloseWindow();
      }
    };
  }

}
