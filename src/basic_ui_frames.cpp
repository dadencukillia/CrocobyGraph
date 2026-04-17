#include "basic_ui_frames.hpp"
#include "imgui.h"
#include "ecs.hpp"
#include "physics.hpp"
#include "physics_system.hpp"

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
    if (mouse_repulsion < 1.0f) return;

    auto& registry = ecs.get_scene().get_registry();


    for(auto [entity, node, pos, vel]: registry.view<const NodeEntity, const PositionComponent, VelocityComponent>().each()) {
      PositionComponent vector = { 
        pos.x - info.mouse_local_x,
        pos.y - info.mouse_local_y,
      };
      float distance_square = vector.x * vector.x + vector.y * vector.y;
      float distance = std::sqrt(distance_square);
      float k = 10'000'000.0f * mouse_repulsion / 20.0f;
      float force_mag = k / (distance_square * distance);
      float mass = node.radius * node.radius;
      vel.x += vector.x * force_mag / mass * info.delta * 60.0f;
      vel.y += vector.y * force_mag / mass * info.delta * 60.0f;
    }
  }

}
