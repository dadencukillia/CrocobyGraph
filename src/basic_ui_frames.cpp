#include "basic_ui_frames.hpp"
#include "imgui.h"
#include "ecs.hpp"
#include "physics_system.hpp"
#include "window_system.hpp"

namespace CrocobyGraph {

  PhysicsFrame::~PhysicsFrame() {}

  void PhysicsFrame::draw(const WindowInfo& info, GraphECS& ecs) {
    bool physics_system_active = ecs.check_system(PhysicsSystem::system_name);

    ImGui::Begin("Physics");

    ImGui::SliderFloat("Mouse Repulsion", &mouse_repulsion, 0.0f, 20.0f);

    if (physics_system_active) {
      ImGui::BeginDisabled();
        ImGui::Checkbox("Jelly Effect", &jelly_toggle);
      ImGui::EndDisabled();

      if (ImGui::Button("Turn off physics")) {
        ecs.remove_systems(PhysicsSystem::system_name);
      }
    } else {
      ImGui::Checkbox("Jelly Effect", &jelly_toggle);

      if (ImGui::Button("Turn on physics")) {
        ecs.add_system(get_physics_system(jelly_toggle));
      }
    }

    ImGui::End();

    if (physics_system_active) apply_mouse_repulsion(info, ecs);
  }

  void PhysicsFrame::apply_mouse_repulsion(const WindowInfo& info, GraphECS& ecs) {
    auto& registry = ecs.get_scene().get_registry();

    for (auto [entity, cursor] : registry.view<CursorEntity>()->each()) {
      registry.emplace_or_replace<RepulsionComponent>(entity, mouse_repulsion);
    }
  }

}
