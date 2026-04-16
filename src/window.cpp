#include "window.hpp"
#include "color.hpp"
#include "components.hpp"
#include "entities.hpp"
#include "entt/entt.hpp"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include <cstdint>
#include <ios>
#include <map>

namespace CrocobyGraph {

  entt::entity create_camera(entt::registry& registry) {
    auto entity = registry.create();
    registry.emplace<CameraEntity>(entity, 1.0f);
    registry.emplace<PositionComponent>(entity, 0.0f, 0.0f);

    return entity;
  }

  Window::Window(Scene* scene, GraphECS* ecs) :
    scene(scene),
    ecs(ecs) 
  {}

  Window::~Window() {
    if (with_gui) rlImGuiShutdown();
    CloseWindow();
  }

  void Window::init(bool gui, Scene* scene, GraphECS* ecs) {
    this->scene = scene;
    this->ecs = ecs;
    init(gui);
  }

  void Window::init(bool gui) {
    with_gui = gui;

    create_camera(scene->get_registry());
    InitWindow(1200, 800, "Graph View");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    if (gui) {
      rlImGuiSetup(true);
      ImGui::GetIO().IniFilename = nullptr;
    }

    painter.load();
  }

  void Window::draw_background() {
    if (window_states.camera_zoom < 0.5) return;

    int64_t spacing = 50;
    int64_t left_rect = static_cast<int>(window_states.camera_x - window_states.width / 2.0 / window_states.camera_zoom);
    int64_t top_rect = static_cast<int>(window_states.camera_y - window_states.height / 2.0 / window_states.camera_zoom);
    int64_t right_rect = static_cast<int>(window_states.camera_x + window_states.width / 2.0 / window_states.camera_zoom);
    int64_t bottom_rect = static_cast<int>(window_states.camera_y + window_states.height / 2.0 / window_states.camera_zoom);

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
    float left_rect = window_states.camera_x - window_states.width / 2.0 / window_states.camera_zoom;
    float top_rect = window_states.camera_y - window_states.height / 2.0 / window_states.camera_zoom;
    float right_rect = window_states.camera_x + window_states.width / 2.0 / window_states.camera_zoom;
    float bottom_rect = window_states.camera_y + window_states.height / 2.0 / window_states.camera_zoom;

    std::unordered_map<entt::entity, PositionComponent> positions;
    std::unordered_map<entt::entity, NodeEntity> nodes;

    auto& registry = scene->get_registry();

    for (auto [entity, node, pos] : registry.view<NodeEntity, PositionComponent>().each()) {
      positions.insert({ entity, pos });
      nodes.insert({ entity, node });
    }

    for (auto [entity, edge] : scene->get_registry().view<EdgeEntity>().each()) {
      auto& a = positions[edge.node_start];
      auto& b = positions[edge.node_end];

      positions.insert({ entity, { (a.x + b.x) / 2.0f, (a.y + b.y) / 2.0f } });

      float border_left = std::min(a.x, b.x) - 3.5f * window_states.camera_zoom;
      float border_right = std::max(a.x, b.x) + 3.5f * window_states.camera_zoom;
      float border_top = std::min(a.y, b.y) - 3.5f * window_states.camera_zoom;
      float border_bottom = std::max(a.y, b.y) + 3.5f * window_states.camera_zoom;

      if (border_left > right_rect || border_right < left_rect || border_top > bottom_rect || border_bottom < top_rect) continue;
      edge.curve_type = EdgeCurveType::Step;

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

    for (auto const& [entity, node] : nodes) {
      auto& pos = positions[entity];

      float border_left = pos.x - node.radius;
      float border_right = pos.x + node.radius;
      float border_top = pos.y - node.radius;
      float border_bottom = pos.y + node.radius;

      if (border_left > right_rect || border_right < left_rect || border_top > bottom_rect || border_bottom < top_rect) continue;

      painter.draw_node({ pos.x, pos.y }, node.color, node.radius);
    }

    for (auto [entity, label, pos] : registry.view<LabelEntity, PositionComponent>().each()) {
      float distance_x = pos.x - window_states.camera_x;
      float distance_y = pos.y - window_states.camera_y;
      float distance = distance_x * distance_x + distance_y * distance_y;
      float max_distance = 40'000.0f / window_states.camera_zoom;

      if (distance > max_distance) continue;

      auto color = label.color;
      color.set_alpha(color.get_alpha() * std::max(0.0f, 1.0f - distance / max_distance));

      painter.draw_label({ pos.x, pos.y }, label.label, color);
    }

    for (auto [entity, label, attach] : registry.view<LabelEntity, AttachComponent>().each()) {
      auto pos = registry.get<PositionComponent>(attach.target);

      float distance_x = pos.x - window_states.camera_x;
      float distance_y = pos.y - window_states.camera_y;
      float distance = distance_x * distance_x + distance_y * distance_y;
      float max_distance = 40'000.0f / window_states.camera_zoom;

      if (distance > max_distance) continue;

      auto color = label.color;
      color.set_alpha(color.get_alpha() * std::max(0.0f, 1.0f - distance / max_distance));

      painter.draw_label({ pos.x + attach.offset_x, pos.y + attach.offset_y }, label.label, color);
    }
  }

  void Window::draw_gui() {
  }

  void Window::draw() {
    Camera2D rl_camera = { 0 };
    rl_camera.target = { window_states.camera_x, window_states.camera_y };
    rl_camera.offset = { static_cast<float>(window_states.width) / 2, static_cast<float>(window_states.height) / 2 };
    rl_camera.zoom = window_states.camera_zoom;
    rl_camera.rotation = 0.0f;

    BeginDrawing();
    ClearBackground(::Color(37, 36, 45));

    BeginMode2D(rl_camera);
      draw_background();
      draw_components();
    EndMode2D();

    if (with_gui) {
      rlImGuiBegin();
      draw_gui();
      rlImGuiEnd();
    }

    EndDrawing();
  }

  void Window::update(double delta) {
    if (WindowShouldClose()) {
      this->ecs->clear_systems();
      return;
    }

    this->window_states.mouse_x = GetMouseX();
    this->window_states.mouse_y = GetMouseY();

    this->window_states.width = GetScreenWidth();
    this->window_states.height = GetScreenHeight();

    this->window_states.left_button_pressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    this->window_states.middle_button_pressed = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);
    this->window_states.right_button_pressed = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    auto& registry = scene->get_registry();
    auto view = registry.view<CameraEntity, PositionComponent>();

    for(auto [entity, camera, pos]: view.each()) {
      if (window_states.middle_button_pressed) {
        SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);

        auto mouse_motion = GetMouseDelta();
        pos.x -= mouse_motion.x / camera.zoom;
        pos.y -= mouse_motion.y / camera.zoom;
      } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
      }

      camera.zoom += -camera.zoom * camera.zoom * -GetMouseWheelMove() * delta * 6;
      camera.zoom = std::min(std::max(camera.zoom, 0.1f), 5.0f);

      this->window_states.camera_x = pos.x;
      this->window_states.camera_y = pos.y;
      this->window_states.camera_zoom = camera.zoom;
      break;
    }
  }

}
