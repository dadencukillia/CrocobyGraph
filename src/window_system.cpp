#include "window_system.hpp"
#include "ecs.hpp"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include <cmath>

namespace CrocobyGraph {

  System<GraphECS> get_window_system(bool debug_gui) {
    return {
      .init_callback = [debug_gui](auto ev) {
        InitWindow(1200, 800, "Graph View");
        if (debug_gui) rlImGuiSetup(true);
      },
      .tick_callback = [debug_gui](auto ev) {
        if (WindowShouldClose()) {
          ev.ecs->clear_systems();
        }

        BeginDrawing();
      
        ClearBackground(::RAYWHITE);
        DrawText("Hello", 0, 0,  16, ::BLACK);

        if (debug_gui) {
          rlImGuiBegin();

          float samples[100];
          for (int n = 0; n < 100; n++)
              samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
          ImGui::PlotLines("Samples", samples, 100);

          ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
          ImGui::BeginChild("Scrolling");
          for (int n = 0; n < 50; n++)
              ImGui::Text("%04d: Some text", n);
          ImGui::EndChild();

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
