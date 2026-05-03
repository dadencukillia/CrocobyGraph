/* I know it's have a lot of same fragments code from bfs_frame.cpp
 * But sorry I currently have a lot of work, so I keep in mind to fix it later
 * TODO: use template pattern or something else to keep DRY principle
 */

#include "dfs_frame.hpp"
#include "../internal/resource_counter.hpp"
#include "../internal/resources.hpp"
#include "../internal/math.hpp"
#include "adjacency_matrix.hpp"
#include "components.hpp"
#include "ecs.hpp"
#include "entities.hpp"
#include "entt/entt.hpp"
#include "imgui.h"
#include "raylib.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <unordered_set>

namespace CrocobyGraph {

  void DFSFrame::reset() {
    frame = 0;
    prev_matrix = { 0 };
    stack = {};
    visited_nodes = {};
    paused = true;
    show_visualization = true;
    visualization_time = 0.0f;
    topology_state = DFSTopologyState::NotSet;
    log = {};
  }

  void DFSFrame::load(GraphECS& ecs) {
    ResourceCounter<MediaControlIcons>::inc();
    ResourceCounter<StarCircleIcon>::inc();
  }

  DFSFrame::~DFSFrame() {
    ResourceCounter<MediaControlIcons>::dec();
    ResourceCounter<StarCircleIcon>::dec();
  }

  void DFSFrame::draw(const WindowInfo& info, GraphECS& ecs) {
    const auto prev_frame { frame };
    const auto& registry { ecs.get_scene().get_registry() };
    ResourceCounter<MediaControlIcons> media_control_icons_res;
    const auto& media_control_icons { media_control_icons_res.get() };
    const auto [nodes, matrix] { ecs.get_scene().adjacency_matrix() };

    ImGui::Begin("DFS", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Button to choose start node
    if (start_node == entt::null) {
      if (start_node_selection) {
        draw_start_selection(info, ecs);

        if (ImGui::Button("Cancel choosing")) start_node_selection = false;
      } else {
        if (ImGui::Button("Choose start node")) {
          start_node_selection = true;
        }
      }
    } else {
      if (registry.valid(start_node)) {
        draw_start_selection(info, ecs);

        if (ImGui::Button("Clear start node")) {
          reset();
          start_node = entt::null;
          ImGui::End();
          return;
        }
      } else {
        reset();
        start_node = entt::null;
      }
    }

    if (start_node != entt::null) {
      // Check topology changes by matrix
      if (topology_state == DFSTopologyState::NotSet) {
        topology_state = DFSTopologyState::Valid;
        prev_matrix = matrix;
      } else if (topology_state == DFSTopologyState::Valid && prev_matrix != matrix) {
        topology_state = DFSTopologyState::Invalid;
        prev_matrix = { 0 };
      } else if (topology_state == DFSTopologyState::Invalid) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Graph topology was changed.\nWe recommend reset the DFS.");
      }

      // UI
      ImGui::Checkbox("Show visualization", &show_visualization);
      if (ImGui::Button("Reset")) {
        start_node = entt::null;
        reset();
        ImGui::End();
        return;
      }

      ImGui::Text("Frame: %d", frame);

      const float frame_interval_seconds { 0.5f };

      if (ImGui::ImageButton("seek_back", static_cast<ImTextureID>(media_control_icons.back_icon.id), ImVec2 { 24.0f, 24.0f }) && frame >= 1) --frame;
      ImGui::SameLine();

      if (paused) {
        if (ImGui::ImageButton("play", static_cast<ImTextureID>(media_control_icons.play_icon.id), ImVec2 { 24.0f, 24.0f })) {
          paused = false;
          visualization_time = frame * frame_interval_seconds;
        }
      } else {
        if (ImGui::ImageButton("pause", static_cast<ImTextureID>(media_control_icons.pause_icon.id), ImVec2 { 24.0f, 24.0f })) paused = true;
      }

      ImGui::SameLine();
      if (ImGui::ImageButton("seek_forward", static_cast<ImTextureID>(media_control_icons.forward_icon.id), ImVec2 { 24.0f, 24.0f })) ++frame;

      // Frames auto increase by time
      if (!paused) {
        visualization_time += info.delta;
        frame = visualization_time / frame_interval_seconds;
      }

      // Visualization on the canvas
      if (show_visualization) {
        // Orders display
        ResourceCounter<FontResource> font_res;
        const auto& font { font_res.get() };

        std::unordered_set<entt::entity> visited_set(visited_nodes.begin(), visited_nodes.end());

        for (const auto node_entity : visited_set) {
          if (registry.valid(node_entity)) {
            const auto& [node, position] = registry.get<const NodeEntity, const PositionComponent>(node_entity);

            const auto translated { translate_world_to_screen_coordinates(
              { position.x, position.y },
              { info.camera_x, info.camera_y },
              info.camera_zoom,
              { static_cast<float>(info.width), static_cast<float>(info.height) }
            ) };

            DrawCircleV(translated, node.radius * info.camera_zoom, Color { 0x00FF0055 });
          }
        }

        if (!stack.empty()) {
          const auto top = stack.top();
          if (registry.valid(top)) {
            const auto& [node, position] = registry.get<const NodeEntity, const PositionComponent>(top);

            const auto translated { translate_world_to_screen_coordinates(
              { position.x, position.y },
              { info.camera_x, info.camera_y },
              info.camera_zoom,
              { static_cast<float>(info.width), static_cast<float>(info.height) }
            ) };

            DrawCircleV(translated, node.radius * info.camera_zoom, Color { 0xFF000055 });
          }
        }
      }

      if (stack.empty()) {
        if (frame == 0) {
          // Initial values
          stack.push(start_node);
          log = { StackAction::Push };
        } else {
          paused = true;
          ImGui::TextColored(ImVec4 { 0.0f, 1.0f, 0.0f, 1.0f }, "Finished");
          frame = log.size() - 1;
        }
      } else if (prev_frame < frame) {
        // When frame increased

        for (size_t i = prev_frame; i < frame && !stack.empty(); ++i) {
          const auto entity = stack.top();
          const auto it { std::ranges::lower_bound(nodes, entity) };
          const size_t matrix_index { static_cast<size_t>(std::distance(nodes.begin(), it)) };

          visited_nodes.push_back(entity);

          bool dead_end { true };
          for (size_t node_index = 0; node_index < nodes.size(); ++node_index) {
            const auto node_id { nodes[node_index] };
            if (!matrix.at(matrix_index, node_index)) continue;
            auto it = std::ranges::find(visited_nodes, node_id);
            if (it != visited_nodes.end()) continue;

            log.push_back(StackAction::Push);
            stack.push(node_id);
            dead_end = false;
            break;
          }

          if (dead_end) {
            log.push_back(StackAction::Pop);
            stack.pop();
          }
        }
      } else if (prev_frame > frame) {
        for (uint32_t i = frame; i < prev_frame; ++i) {
          if (log.back() == StackAction::Push) {
            stack.pop();
            visited_nodes.pop_back();
          } else {
            stack.push(visited_nodes.back());
            visited_nodes.pop_back();
          }
          log.pop_back();
        }
      }
    }

    ImGui::End();
  }

  void DFSFrame::draw_start_selection(const WindowInfo& info, GraphECS& ecs) {
    ResourceCounter<StarCircleIcon> star_circle_icon_res;
    const StarCircleIcon& star_circle_icon { star_circle_icon_res.get() };

    const auto& registry { ecs.get_scene().get_registry() };

    const auto draw_at_pos = [&info, &star_circle_icon](Vector2 position) {
      const auto translated { translate_world_to_screen_coordinates(
        position,
        { info.camera_x, info.camera_y }, 
        info.camera_zoom, 
        { static_cast<float>(info.width), static_cast<float>(info.height) }
      ) };
      DrawTextureEx(star_circle_icon.star_circle_icon, { 
        translated.x - star_circle_icon.star_circle_icon.width * 0.5f,
        translated.y - star_circle_icon.star_circle_icon.height * 0.5f
      }, 0.0f, 1.0f, { 255, 255, 255, 255 });
    };

    if (start_node_selection) {
      for (const auto& [entity, node, position] : registry.view<const NodeEntity, const PositionComponent>().each()) {
        if (
          position.x - node.radius <= info.mouse_local_x && position.x + node.radius >= info.mouse_local_x &&
          position.y - node.radius <= info.mouse_local_y && position.y + node.radius >= info.mouse_local_y
        ) {
          draw_at_pos({ position.x, position.y });

          if (info.left_button_down && !ImGui::GetIO().WantCaptureMouse) {
            start_node = entity;
            start_node_selection = false;
          }

          break;
        }
      }
    } else {
      const auto& position = registry.get<const PositionComponent>(start_node);

      draw_at_pos({ position.x, position.y });
    }
  }

}
