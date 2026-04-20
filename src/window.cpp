#include "window.hpp"
#include "color.hpp"
#include "components.hpp"
#include "config.hpp"
#include "entities.hpp"
#include "entt/entt.hpp"
#include "math.hpp"
#include "physics_system.hpp"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "window_system.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace CrocobyGraph {

  entt::entity create_camera(entt::registry& registry) {
    auto entity = registry.create();
    registry.emplace<CameraEntity>(entity, 1.0f);
    registry.emplace<PositionComponent>(entity, 0.0f, 0.0f);

    return entity;
  }

  entt::entity create_cursor(entt::registry& registry) {
    auto entity = registry.create();
    registry.emplace<CursorEntity>(entity, false, false, false);
    registry.emplace<PositionComponent>(entity, 0.0f, 0.0f);

    return entity;
  }

  Camera2D get_camera_2d(const WindowInfo& info) {
    return {
      .offset = { static_cast<float>(info.width) / 2, static_cast<float>(info.height) / 2 },
      .target = { info.camera_x, info.camera_y },
      .rotation = 0.f,
      .zoom = info.camera_zoom,
    };
  }

  Window::~Window() {
    auto& registry = scene->get_registry();

    registry.clear<CameraEntity>();
    registry.clear<CursorEntity>();

    if (!ui_frames.empty()) rlImGuiShutdown();
    CloseWindow();
  }

  void Window::init(std::vector<std::unique_ptr<WindowUIFrame>>&& ui_frames, Scene* scene, GraphECS* ecs) {
    this->scene = scene;
    this->ecs = ecs;
    this->ui_frames = std::move(ui_frames);

    InitWindow(1200, 800, "Graph View");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    create_camera(scene->get_registry());
    create_cursor(scene->get_registry());

    if (!this->ui_frames.empty()) {
      rlImGuiSetup(true);
      ImGui::GetIO().IniFilename = nullptr;

      for (auto& frame : this->ui_frames) {
        frame->load(*ecs);
      }
    }

    painter.load();
  }

  void Window::draw_background() {
    if (window_states.camera_zoom < 0.5) return;

    int64_t spacing = 50;
    int64_t left_rect = static_cast<int>(window_states.camera_border_left);
    int64_t top_rect = static_cast<int>(window_states.camera_border_top);
    int64_t right_rect = static_cast<int>(window_states.camera_border_right);
    int64_t bottom_rect = static_cast<int>(window_states.camera_border_bottom);

    left_rect -= 2;
    top_rect -= 2;
    left_rect -= left_rect % spacing;
    top_rect -= top_rect % spacing;
    right_rect += 2;
    bottom_rect += 2;

    for (int64_t x = left_rect; x <= right_rect; x += spacing) {
      for (int64_t y = top_rect; y <= bottom_rect; y += spacing) {
        DrawRectangle(x - 1, y - 1, 2, 2, ::Color(255, 255, 255, 100));
      }
    }
  }

  void Window::draw_components() {
    std::unordered_map<entt::entity, PositionComponent> positions;
    std::unordered_map<entt::entity, NodeEntity> nodes;

    auto& registry = scene->get_registry();

    Vector2 camera_top_left { window_states.camera_border_left, window_states.camera_border_top };
    Vector2 camera_bottom_right { window_states.camera_border_right, window_states.camera_border_bottom };

    // Store nodes in maps
    for (auto [entity, node, pos] : registry.view<const NodeEntity, const PositionComponent>().each()) {
      positions.insert({ entity, pos });
      nodes.insert({ entity, node });
    }

    // Draw edges (below nodes)
    for (auto [entity, edge] : scene->get_registry().view<const EdgeEntity>().each()) {
      auto& a = positions[edge.node_start];
      auto& b = positions[edge.node_end];

      if (a.x == b.x && a.y == b.y) {
        auto radius = nodes[edge.node_start].radius;
        float distance = std::sqrt(a.x * a.x + a.y * a.y);
        Vector2 normalized = { a.x / distance, a.y / distance };
        float length = radius * 4.0f;
        Vector2 corner = { a.x + normalized.x * length, a.y + normalized.y * length };

        positions.insert({ entity, { corner.x * 2 / 3, corner.y * 2 / 3 } });

        float border_left = std::min(a.x, corner.x);
        float border_right = std::max(a.x, corner.x);
        float border_top = std::min(a.y, corner.y);
        float border_bottom = std::max(a.y, corner.y);

        if (!check_rect_a_in_rect_b({ border_left, border_top }, { border_right, border_bottom }, camera_top_left, camera_bottom_right)) continue;

        painter.draw_self_loop({ a.x, a.y }, edge.color, nodes[edge.node_start].radius);

        if (edge.arrow_on_start || edge.arrow_on_end) {
          auto radius = nodes[edge.node_start].radius;
          painter.draw_arrow({ b.x, b.y }, { a.x, a.y }, radius, edge.color, edge.curve_type);
        }
      } else {
        positions.insert({ entity, { (a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f } });

        float border_left = std::min(a.x, b.x) - 3.5f * window_states.camera_zoom;
        float border_right = std::max(a.x, b.x) + 3.5f * window_states.camera_zoom;
        float border_top = std::min(a.y, b.y) - 3.5f * window_states.camera_zoom;
        float border_bottom = std::max(a.y, b.y) + 3.5f * window_states.camera_zoom;

        if (!check_rect_a_in_rect_b({ border_left, border_top }, { border_right, border_bottom }, camera_top_left, camera_bottom_right)) continue;

        painter.draw_edge({ a.x, a.y }, { b.x, b.y }, edge.color, edge.curve_type);

        if (edge.arrow_on_start) {
          auto radius = nodes[edge.node_start].radius;
          painter.draw_arrow({ b.x, b.y }, { a.x, a.y }, radius, edge.color, edge.curve_type);
        }

        if (edge.arrow_on_end) {
          auto radius = nodes[edge.node_end].radius;
          painter.draw_arrow({ a.x, a.y }, { b.x, b.y }, radius, edge.color, edge.curve_type);
        }
      }
    }

    // Draw nodes from maps
    for (auto const& [entity, node] : nodes) {
      auto& pos = positions[entity];

      if (registry.all_of<JellyComponent>(entity)) {
        float border_left = pos.x - node.radius;
        float border_right = pos.x + node.radius;
        float border_top = pos.y - node.radius;
        float border_bottom = pos.y + node.radius;
        auto& jelly = registry.get<JellyComponent>(entity);

        for (auto& point : jelly.points) {
          border_left = std::min(point.x, border_left);
          border_right = std::max(point.x, border_right);
          border_top = std::min(point.y, border_top);
          border_bottom = std::max(point.y, border_bottom);
        }

        if (!check_rect_a_in_rect_b({ border_left, border_top }, { border_right, border_bottom }, camera_top_left, camera_bottom_right)) continue;

        painter.draw_jelly_node(jelly.points, { pos.x, pos.y }, node.color);
      } else {
        float border_left = pos.x - node.radius;
        float border_right = pos.x + node.radius;
        float border_top = pos.y - node.radius;
        float border_bottom = pos.y + node.radius;

        if (!check_rect_a_in_rect_b({ border_left, border_top }, { border_right, border_bottom }, camera_top_left, camera_bottom_right)) continue;

        painter.draw_node({ pos.x, pos.y }, node.color, node.radius);
      }
    }

    // Draw labels
    for (auto [entity, label, pos] : registry.view<const LabelEntity, const PositionComponent>().each()) {
      float distance_x = pos.x - window_states.camera_x;
      float distance_y = pos.y - window_states.camera_y;
      float distance = distance_x * distance_x + distance_y * distance_y;
      float max_distance = 40'000.0f / window_states.camera_zoom;

      if (distance > max_distance) continue;

      auto color = label.color;
      color.set_alpha(color.get_alpha() * std::max(0.0f, 1.0f - distance / max_distance));

      painter.draw_label({ pos.x, pos.y }, label.label, color);
    }

    for (auto [entity, label, attach] : registry.view<const LabelEntity, const AttachComponent>().each()) {
      auto pos = registry.get<PositionComponent>(attach.target);

      float distance_x = pos.x - window_states.camera_x;
      float distance_y = pos.y - window_states.camera_y;
      float distance = distance_x * distance_x + distance_y * distance_y;
      float max_distance = LABELS_VISIBLE_DISTANCE * LABELS_VISIBLE_DISTANCE / (window_states.camera_zoom * window_states.camera_zoom);

      if (distance > max_distance) continue;

      auto color = label.color;
      color.set_alpha(color.get_alpha() * std::max(0.0f, 1.0f - distance / max_distance));

      painter.draw_label({ pos.x + attach.offset_x, pos.y + attach.offset_y }, label.label, color);
    }
  }

  void Window::draw_gui() {
    rlImGuiBegin();

    for (auto& frames : ui_frames) {
      frames->draw(window_states, *ecs);
    }

    rlImGuiEnd();
  }

  void Window::draw() {
    BeginDrawing();
    ClearBackground(::Color(37, 36, 45));

    BeginMode2D(get_camera_2d(window_states));
      draw_background();
      draw_components();
    EndMode2D();

    if (!ui_frames.empty()) draw_gui();

    EndDrawing();
  }

  void Window::update(double delta) {
    window_states.delta = delta;
    window_states.mouse_x = GetMouseX();
    window_states.mouse_y = GetMouseY();

    window_states.width = GetScreenWidth();
    window_states.height = GetScreenHeight();

    window_states.left_button_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    window_states.middle_button_down = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);
    window_states.right_button_down = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    auto& registry = scene->get_registry();

    for(auto [entity, camera, pos] : registry.view<CameraEntity, PositionComponent>().each()) {
      if (window_states.middle_button_down) {
        SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);

        auto mouse_motion = GetMouseDelta();
        pos.x -= mouse_motion.x / camera.zoom;
        pos.y -= mouse_motion.y / camera.zoom;
      } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
      }

      camera.zoom += -camera.zoom * camera.zoom * -GetMouseWheelMove() * delta * 6;
      camera.zoom = std::min(std::max(camera.zoom, 0.1f), 5.0f);

      window_states.camera_x = pos.x;
      window_states.camera_y = pos.y;
      window_states.camera_zoom = camera.zoom;
      window_states.camera_border_left = window_states.camera_x - window_states.width / 2.0 / window_states.camera_zoom;
      window_states.camera_border_top = window_states.camera_y - window_states.height / 2.0 / window_states.camera_zoom;
      window_states.camera_border_right = window_states.camera_x + window_states.width / 2.0 / window_states.camera_zoom;
      window_states.camera_border_bottom = window_states.camera_y + window_states.height / 2.0 / window_states.camera_zoom;
      window_states.mouse_local_x = window_states.camera_border_left + GetMouseX() / camera.zoom;
      window_states.mouse_local_y = window_states.camera_border_top + GetMouseY() / camera.zoom;

      break;
    }

    for(auto [entity, cursor, pos] : registry.view<CursorEntity, PositionComponent>().each()) {
      cursor.left_button = window_states.left_button_down;
      cursor.middle_button = window_states.middle_button_down;
      cursor.right_button = window_states.right_button_down;
      pos.x = window_states.mouse_local_x;
      pos.y = window_states.mouse_local_y;
    }
  }

}
