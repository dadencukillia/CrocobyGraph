#include "editor_frame.hpp"
#include "components.hpp"
#include "ecs.hpp"
#include "entities.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "imgui.h"
#include "math.hpp"
#include "raylib.h"
#include "resources/editor_icons.hpp"
#include <cassert>
#include <optional>
#include <string_view>

namespace CrocobyGraph {

  struct IconTextures {
    Texture2D view_icon;
    Texture2D node_icon;
    Texture2D edge_icon;
    Texture2D label_icon;
  };
  
  EditorFrame::~EditorFrame() {
    UnloadTexture(textures->view_icon);
    UnloadTexture(textures->node_icon);
    UnloadTexture(textures->edge_icon);
    UnloadTexture(textures->label_icon);
    delete textures;
  }

  void EditorFrame::load(GraphECS& ecs) {
    textures = new IconTextures();

    auto eye_image = LoadImageFromMemory(".png", EditorIconEyeData, EditorIconEyeSize);
    auto node_image = LoadImageFromMemory(".png", EditorIconNodeData, EditorIconNodeSize);
    auto edge_image = LoadImageFromMemory(".png", EditorIconEdgeData, EditorIconEdgeSize);
    auto label_image = LoadImageFromMemory(".png", EditorIconLabelData, EditorIconLabelSize);

    textures->view_icon = LoadTextureFromImage(eye_image);
    textures->node_icon = LoadTextureFromImage(node_image);
    textures->edge_icon = LoadTextureFromImage(edge_image);
    textures->label_icon = LoadTextureFromImage(label_image);

    UnloadImage(eye_image);
    UnloadImage(node_image);
    UnloadImage(edge_image);
    UnloadImage(label_image);
  }

  inline std::optional<entt::entity> get_node_connection(const entt::registry& registry, const entt::entity& node_a, const entt::entity& node_b) {
    for (auto [entity, edge] : registry.view<EdgeEntity>()->each()) {
      if ((edge.node_start == node_a && edge.node_end == node_b) || (edge.node_start == node_b && edge.node_end == node_a)) {
        return entity;
      }
    }

    return std::nullopt;
  }

  bool draw_toggle_icon_button(const char* str_id, unsigned int tex_id, ImVec2 size, bool on) {
    if (on) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
    auto clicked = ImGui::ImageButton(str_id, static_cast<ImTextureID>(tex_id), size);
    if (on) ImGui::PopStyleColor();

    return clicked;
  }

  void EditorFrame::draw(const WindowInfo& info, GraphECS& ecs) {
    assert(textures != nullptr);

    auto& registry = ecs.get_scene().get_registry();

    auto current_view = editor_mode == EditMode::View;
    auto current_node = editor_mode == EditMode::Node;
    auto current_edge = editor_mode == EditMode::Edge;
    auto current_label = editor_mode == EditMode::Label;

    ImGui::Begin("Editor");

    ImGui::Text("x: %d, y: %d", static_cast<int>(info.mouse_local_x), static_cast<int>(info.mouse_local_y));

    bool toggle_view, toggle_node, toggle_edge, toggle_label;
    draw_mode_toolbar(toggle_view, toggle_node, toggle_edge, toggle_label, current_view, current_node, current_edge, current_label);

    if (selection.empty() && temp_selection.empty()) {
      if (current_node || current_edge || current_label) ImGui::Text("Select - left button\nCreate - right click");
    } else {
      ImGui::Text("Selected: %lu", selection.size() + temp_selection.size());
      if (!drag.dragging) ImGui::Text("Hold right button to move");

      if (ImGui::Button("Delete")) {
        for (auto& select : selection) {
          registry.destroy(select);
        }
        selection.clear();
      }

      if (selection.size() == 2) {
        auto edge_connection = get_node_connection(registry, *selection.begin(), *std::next(selection.begin()));
        ImGui::SameLine();
        if (edge_connection.has_value()) {
          if (ImGui::Button("Disconnect")) {
            registry.destroy(edge_connection.value());
          }
        } else {
          if (ImGui::Button("Connect")) {
            auto entity = registry.create();
            registry.emplace<EdgeEntity>(entity, EdgeEntity {
              .node_start = *selection.begin(),
              .node_end = *std::next(selection.begin()),
            });
          }
        }
      }
    }

    ImGui::End();

    process_mode_toggle(toggle_view, toggle_node, toggle_edge, toggle_label);
    process_selection(info, ecs, current_view, current_node, current_edge, current_label);
  }

  inline void EditorFrame::draw_mode_toolbar(bool& toggle_view, bool& toggle_node, bool& toggle_edge, bool& toggle_label, bool current_view, bool current_node, bool current_edge, bool current_label) {
    toggle_view = draw_toggle_icon_button("editor_view_icon", static_cast<ImTextureID>(textures->view_icon.id), ImVec2(24.0f, 24.0f), current_view) && !current_view;
    ImGui::SameLine();
    toggle_node = draw_toggle_icon_button("editor_node_icon", static_cast<ImTextureID>(textures->node_icon.id), ImVec2(24.0f, 24.0f), current_node) && !current_node;
    ImGui::SameLine();
    toggle_edge = draw_toggle_icon_button("editor_edge_icon", static_cast<ImTextureID>(textures->edge_icon.id), ImVec2(24.0f, 24.0f), current_edge) && !current_edge;
    ImGui::SameLine();
    toggle_label = draw_toggle_icon_button("editor_label_icon", static_cast<ImTextureID>(textures->label_icon.id), ImVec2(24.0f, 24.0f), current_label) && !current_label;

    std::string_view mode_name = "";
    switch (editor_mode) {
    case EditMode::View: mode_name = "View"; break;
    case EditMode::Node: mode_name = "Node"; break;
    case EditMode::Edge: mode_name = "Edge"; break;
    case EditMode::Label: mode_name = "Label"; break;
    }

    ImGui::Text("Mode: %s", mode_name.data());
  }

  inline void EditorFrame::process_mode_toggle(bool toggle_view, bool toggle_node, bool toggle_edge, bool toggle_label) {
    if (toggle_view || toggle_node || toggle_edge || toggle_label) {
      drag.dragging = false;
      selection.clear();

      if (toggle_view) {
        editor_mode = EditMode::View;
      } else if (toggle_node) {
        editor_mode = EditMode::Node;
      } else if (toggle_edge) {
        editor_mode = EditMode::Edge;
      } else if (toggle_label) {
        editor_mode = EditMode::Label;
      }
    }
  }

  inline void EditorFrame::process_selection(const WindowInfo& info, GraphECS& ecs, bool current_view, bool current_node, bool current_edge, bool current_label) {
    if (current_view) return;

    auto& registry = ecs.get_scene().get_registry();

    if (info.left_button_down && !ImGui::GetIO().WantCaptureMouse) {
      if (drag.dragging) {
        drag.end_x = info.mouse_local_x;
        drag.end_y = info.mouse_local_y;
      } else {
        drag.dragging = true;
        drag.start_x = info.mouse_local_x;
        drag.start_y = info.mouse_local_y;
        drag.end_x = info.mouse_local_x;
        drag.end_y = info.mouse_local_y;
      }

      Vector2 corner_top_left = { std::min(drag.start_x, drag.end_x), std::min(drag.start_y, drag.end_y) };
      Vector2 corner_bottom_right = { std::max(drag.start_x, drag.end_x), std::max(drag.start_y, drag.end_y) };

      Vector2 translated_corner = translate_world_to_screen_coordinates(corner_top_left, { info.camera_x, info.camera_y }, info.camera_zoom, { static_cast<float>(info.width), static_cast<float>(info.height) });
      Vector2 translated_size = { (corner_bottom_right.x - corner_top_left.x) * info.camera_zoom, (corner_bottom_right.y - corner_top_left.y) * info.camera_zoom };

      DrawRectangleV(
        translated_corner,
        translated_size,
        { 50, 50, 255, 50 }
      );

      DrawRectangleLinesEx(Rectangle { translated_corner.x, translated_corner.y, translated_size.x, translated_size.y }, 1.0f, { 0, 0, 180, 100 });

      if (current_node) {
        if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_LEFT_SHIFT)) {
          selection.clear();
        }
        temp_selection.clear();
        for (auto [entity, node, pos] : registry.view<const NodeEntity, const PositionComponent>().each()) {
          if (pos.x + node.radius >= corner_top_left.x && pos.x - node.radius <= corner_bottom_right.x && pos.y + node.radius >= corner_top_left.y && pos.y - node.radius <= corner_bottom_right.y) {
            if (!selection.contains(entity)) {
              temp_selection.insert(entity);
            }
          }
        }
      }
    } else {
      for (auto& temp_select : temp_selection) {
        selection.insert(temp_select);
      }
      temp_selection.clear();
      drag.dragging = false;
    }

    if (current_node) {
      for (auto* selection_list : { &selection, &temp_selection }) {
        for (auto& selected : *selection_list) {
          if (!registry.valid(selected)) continue;

          auto [node, pos] = registry.get<const NodeEntity, const PositionComponent>(selected);

          DrawCircleV(
            translate_world_to_screen_coordinates({ pos.x, pos.y }, { info.camera_x, info.camera_y }, info.camera_zoom, { static_cast<float>(info.width), static_cast<float>(info.height) }), 
            node.radius * info.camera_zoom, { 0, 0, 180, 100 }
          );
        }
      }
    }
  }

}
