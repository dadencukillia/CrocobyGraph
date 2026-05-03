#include "bfs_frame.hpp"
#include "../internal/resource_counter.hpp"
#include "../internal/resources.hpp"
#include "../internal/math.hpp"
#include "adjacency_matrix.hpp"
#include "components.hpp"
#include "ecs.hpp"
#include "entities.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "imgui.h"
#include "raylib.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>

namespace CrocobyGraph {

  void BFSFrame::reset() {
    frame = 0;
    prev_matrix = { 0 };
    queue = {};
    visited_nodes = {};
    visited_orders = {};
    paused = true;
    show_visualization = true;
    visualization_time = 0.0f;
    labels_pool = {};
    saved_path = {};
    topology_state = TopologyState::NotSet;
  }

  void BFSFrame::load(GraphECS& ecs) {
    ResourceCounter<MediaControlIcons>::inc();
    ResourceCounter<FlagCircleIcon>::inc();
    ResourceCounter<StarCircleIcon>::inc();
  }

  BFSFrame::~BFSFrame() {
    ResourceCounter<MediaControlIcons>::dec();
    ResourceCounter<FlagCircleIcon>::dec();
    ResourceCounter<StarCircleIcon>::dec();
  }

  void BFSFrame::draw(const WindowInfo& info, GraphECS& ecs) {
    const auto prev_frame { frame };
    const auto& registry { ecs.get_scene().get_registry() };
    ResourceCounter<MediaControlIcons> media_control_icons_res;
    const auto& media_control_icons { media_control_icons_res.get() };
    const auto [nodes, matrix] { ecs.get_scene().adjacency_matrix() };

    ImGui::Begin("BFS", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Button to choose start node
    if (start_node == entt::null) {
      if (start_node_selection) {
        draw_start_selection(info, ecs);

        if (ImGui::Button("Cancel choosing")) start_node_selection = false;
      } else {
        if (ImGui::Button("Choose start node")) {
          start_node_selection = true;
          finish_node_selection = false;
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

    // Button to choose finish node
    if (finish_node == entt::null) {
      if (finish_node_selection) {
        draw_finish_selection(info, ecs);

        if (ImGui::Button("Cancel choosing")) finish_node_selection = false;
      } else {
        if (ImGui::Button("Choose finish node")) {
          finish_node_selection = true;
          start_node_selection = false;
        }
      }
    } else {
      if (registry.valid(finish_node)) {
        draw_finish_selection(info, ecs);

        if (ImGui::Button("Clear finish node")) {
          reset();
          finish_node = entt::null;
          ImGui::End();
          return;
        }
      } else {
        reset();
        finish_node = entt::null;
      }
    }

    // When both nodes are chosen
    if (start_node != entt::null && finish_node != entt::null) {
      // Check topology changes by matrix
      if (topology_state == TopologyState::NotSet) {
        topology_state = TopologyState::Valid;
        prev_matrix = matrix;
      } else if (topology_state == TopologyState::Valid && prev_matrix != matrix) {
        topology_state = TopologyState::Invalid;
        prev_matrix = { 0 };
      } else if (topology_state == TopologyState::Invalid) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Graph topology was changed.\nWe recommend reset the BFS.");
      }

      // UI
      ImGui::Checkbox("Show visualization", &show_visualization);
      if (ImGui::Button("Reset")) {
        start_node = entt::null;
        finish_node = entt::null;
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

        for (size_t i = 1; i < visited_nodes.size(); ++i) {
          const auto order { visited_orders[i] };
          const auto node { visited_nodes[i] };
          if (!registry.valid(node)) continue;

          const auto measure = MeasureTextEx(font.open_sans, labels_pool[order - 1].data(), LABEL_FONT_SIZE * info.camera_zoom, LABEL_SPACING);

          const auto& position = registry.get<const PositionComponent>(node);
          const auto translated { translate_world_to_screen_coordinates(
            { position.x, position.y },
            { info.camera_x, info.camera_y },
            info.camera_zoom,
            { static_cast<float>(info.width), static_cast<float>(info.height) }
          ) };

          DrawTextEx(
            font.open_sans,
            labels_pool[order - 1].data(),
            { translated.x - measure.x * 0.5f, translated.y - measure.y * 0.5f },
            LABEL_FONT_SIZE * info.camera_zoom, LABEL_SPACING, Color { 0xFF0000FF }
          );
        }

        // Shortest path display
        if (!saved_path.empty()) {
          Vector2 last_pos {};
          bool first { true };

          for (const auto node_entity : saved_path) {
            if (!registry.valid(node_entity)) {
              saved_path.clear();
              break;
            }

            const auto position = registry.get<const PositionComponent>(node_entity);
            const auto translated { translate_world_to_screen_coordinates(
              { position.x, position.y },
              { info.camera_x, info.camera_y },
              info.camera_zoom,
              { static_cast<float>(info.width), static_cast<float>(info.height) }
            ) };

            if (first) {
              last_pos = translated;
              first = false;
            } else [[likely]] {
              DrawLineEx(translated, last_pos, EDGES_THICKNESS * info.camera_zoom * 2.0f, Color { 0xFF0000FF });
              last_pos = translated;
            }
          }
        }
      }

      if (queue.empty()) {
        if (frame == 0) {
          // Initial values
          queue.push({ 0, start_node });
          visited_nodes = { start_node };
          visited_orders = { 0 };
          labels_pool.clear();
        } else {
          paused = true;
          frame = visited_orders.back() + 1;
          ImGui::TextColored(ImVec4 { 1.0f, 0.0f, 0.0f, 1.0f }, "Target isn't reachable");
        }
      } else if (prev_frame < frame) {
        // When frame increased

        auto it = std::ranges::find(visited_nodes, finish_node);
        if (it == visited_nodes.end()) {
          // When we haven't reached the target
          // BFS calculations

          while (!queue.empty()) {
            const auto& queue_item = queue.front();
            const auto order = queue_item.first;
            const auto entity = queue_item.second;
            const auto it { std::ranges::lower_bound(nodes, entity) };
            const size_t matrix_index { static_cast<size_t>(std::distance(nodes.begin(), it)) };

            if (order < prev_frame || order >= frame) break;

            for (size_t node_index = 0; node_index < nodes.size(); ++node_index) {
              const auto node_id { nodes[node_index] };
              if (!matrix.at(matrix_index, node_index)) continue;
              auto it = std::ranges::find(visited_nodes, node_id);
              if (it != visited_nodes.end()) continue;

              queue.push({ order + 1, node_id });
              visited_nodes.push_back(node_id);
              visited_orders.push_back(order + 1);

              if (node_id == finish_node) {
                frame = order + 1;
                paused = true;
                saved_path = build_nodes_path(nodes, matrix);
                goto found_target_node;
              }
            }

            queue.pop();
          }

          found_target_node:

          for (size_t i = labels_pool.size() + 1; i <= frame; ++i) {
            labels_pool.push_back(std::to_string(i));
          }
        } else {
          frame = visited_orders[it - visited_nodes.begin()];
          paused = true;
        }
      } else if (prev_frame > frame) {
        queue = {};
        for (size_t i = visited_orders.size(); i > 0;) {
          --i;
          if (visited_orders[i] == frame) {
            queue.push({ frame, visited_nodes[i] });
          } else if (visited_orders[i] > frame) {
            visited_orders.erase(visited_orders.begin() + i);
            visited_nodes.erase(visited_nodes.begin() + i);
          } else break;
        }

        for (size_t i = labels_pool.size(); i > frame;) {
          --i;
          labels_pool.erase(labels_pool.begin() + i);
        }

        if (!saved_path.empty()) saved_path.clear();
      }
    }

    ImGui::End();
  }

  std::vector<entt::entity> BFSFrame::build_nodes_path(const std::vector<entt::entity>& nodes_map, const AdjacencyMatrix& matrix) {
    std::vector<entt::entity> path;
    uint32_t order { visited_orders.back() };
    path = { finish_node };
    path.reserve(order + 1);

    size_t order_last_index { static_cast<size_t>(std::ranges::find_last(visited_orders, order - 1).begin() - visited_orders.begin()) };
    auto prev_node { path.back() };
    auto prev_it { std::ranges::lower_bound(nodes_map, prev_node) };
    size_t prev_mat_index { static_cast<size_t>(std::distance(nodes_map.begin(), prev_it)) };
    for (uint32_t order_back = order - 1; order_back > 0; --order_back) {
      size_t last_node_index { order_last_index };
      for (size_t node_index = order_last_index; node_index > 0; --node_index) {
        last_node_index = node_index;
        if (visited_orders[node_index] != order_back) break;

        const auto cur_node { visited_nodes[node_index] };
        const auto cur_it { std::ranges::lower_bound(nodes_map, cur_node) };
        const size_t cur_mat_index { static_cast<size_t>(std::distance(nodes_map.begin(), cur_it)) };
        if (matrix.at(cur_mat_index, prev_mat_index)) {
          path.push_back(cur_node);
          prev_node = cur_node;
          prev_it = cur_it;
          prev_mat_index = cur_mat_index;
          break;
        }
      }

      // Find last node of the previous order to start with in the next iteration
      for (size_t node_index = last_node_index; node_index > 0; --node_index) {
        if (visited_orders[node_index] != order_back) {
          order_last_index = node_index;
          break;
        }
      }
    }

    path.push_back(start_node);
    return path;
  }

  void BFSFrame::draw_start_selection(const WindowInfo& info, GraphECS& ecs) {
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
        if (entity == finish_node) continue;

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

  void BFSFrame::draw_finish_selection(const WindowInfo& info, GraphECS& ecs) {
    ResourceCounter<FlagCircleIcon> flag_circle_icon_res;
    const FlagCircleIcon& flag_circle_icon { flag_circle_icon_res.get() };

    const auto& registry { ecs.get_scene().get_registry() };

    const auto draw_at_pos = [&info, &flag_circle_icon](Vector2 position) {
      const auto translated { translate_world_to_screen_coordinates(
        position,
        { info.camera_x, info.camera_y }, 
        info.camera_zoom, 
        { static_cast<float>(info.width), static_cast<float>(info.height) }
      ) };
      DrawTextureEx(flag_circle_icon.flag_circle_icon, { 
        translated.x - flag_circle_icon.flag_circle_icon.width * 0.5f,
        translated.y - flag_circle_icon.flag_circle_icon.height * 0.5f
      }, 0.0f, 1.0f, { 255, 255, 255, 255 });
    };

    if (finish_node_selection) {
      for (const auto& [entity, node, position] : registry.view<const NodeEntity, const PositionComponent>().each()) {
        if (entity == start_node) continue;

        if (
          position.x - node.radius <= info.mouse_local_x && position.x + node.radius >= info.mouse_local_x &&
          position.y - node.radius <= info.mouse_local_y && position.y + node.radius >= info.mouse_local_y
        ) {
          draw_at_pos({ position.x, position.y });

          if (info.left_button_down && !ImGui::GetIO().WantCaptureMouse) {
            finish_node = entity;
            finish_node_selection = false;
          }

          break;
        }
      }
    } else {
      const auto& position = registry.get<const PositionComponent>(finish_node);

      draw_at_pos({ position.x, position.y });
    }
  }

}
