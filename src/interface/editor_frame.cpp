#include "editor_frame.hpp"
#include "../internal/painter.hpp"
#include "../internal/window.hpp"
#include "../internal/math.hpp"
#include "../internal/calc_impls.hpp"
#include "../internal/resource_counter.hpp"
#include "../internal/labels.hpp"
#include "../resources/editor_icons.hpp"
#include "../config.hpp"
#include "components.hpp"
#include "ecs.hpp"
#include "entities.hpp"
#include "entt/entt.hpp"
#include "physics_system.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <string_view>
#include "../internal/resources.hpp"
#include "imgui.h"
#include "raylib.h"

namespace CrocobyGraph {

  struct IconTextures {
    Texture2D view_icon;
    Texture2D node_icon;
    Texture2D edge_icon;
    Texture2D label_icon;

    IconTextures() {
      auto eye_image = LoadImageFromMemory(".png", EditorIconEyeData, EditorIconEyeSize);
      auto node_image = LoadImageFromMemory(".png", EditorIconNodeData, EditorIconNodeSize);
      auto edge_image = LoadImageFromMemory(".png", EditorIconEdgeData, EditorIconEdgeSize);
      auto label_image = LoadImageFromMemory(".png", EditorIconLabelData, EditorIconLabelSize);

      view_icon = LoadTextureFromImage(eye_image);
      node_icon = LoadTextureFromImage(node_image);
      edge_icon = LoadTextureFromImage(edge_image);
      label_icon = LoadTextureFromImage(label_image);

      UnloadImage(eye_image);
      UnloadImage(node_image);
      UnloadImage(edge_image);
      UnloadImage(label_image);

      std::cout << "Editor frame icons loaded\n";
    }

    ~IconTextures() {
      UnloadTexture(view_icon);
      UnloadTexture(node_icon);
      UnloadTexture(edge_icon);
      UnloadTexture(label_icon);

      std::cout << "Editor frame icons unloaded\n";
    }
  };

  EditorFrame::EditorFrame() {
    ResourceCounter<IconTextures>::inc();
  }

  EditorFrame::~EditorFrame() {
    ResourceCounter<IconTextures>::dec();
  }
  
  void EditorFrame::load(GraphECS& ecs) {}

  inline std::optional<entt::entity> get_node_connection(const entt::registry& registry, const entt::entity& node_a, const entt::entity& node_b) {
    for (const auto& [entity, edge] : registry.view<const EdgeEntity>().each()) {
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
    auto& registry = ecs.get_scene().get_registry();

    auto current_view = editor_mode == EditMode::View;
    auto current_node = editor_mode == EditMode::Node;
    auto current_edge = editor_mode == EditMode::Edge;
    auto current_label = editor_mode == EditMode::Label;

    ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("CURSOR x: %d, y: %d", static_cast<int>(info.mouse_local_x), static_cast<int>(info.mouse_local_y));

    bool toggle_view, toggle_node, toggle_edge, toggle_label;
    draw_mode_toolbar(toggle_view, toggle_node, toggle_edge, toggle_label, current_view, current_node, current_edge, current_label);

    if (current_view) {
      ImGui::Text("Nothing happens special\nin the view mode");
    } else {
      if (!selection.empty() || !temp_selection.empty()) {
        if (ImGui::Button("Delete")) {
          for (auto& select : selection) {
            registry.destroy(select);
          }
          selection.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button("Deselect")) {
          selection.clear();
        }

        ImGui::Text("Selected: %lu", selection.size() + temp_selection.size());
      }

      process_selection(info, ecs, current_node, current_edge, current_label);
      process_motion(info, ecs, current_node, current_edge, current_label);

      if (current_node) draw_node_specific_toolbar(info, ecs);
      else if (current_edge) draw_edge_specific_toolbar(info, ecs);
      else if (current_label) draw_label_specific_toolbar(info, ecs);
    }

    ImGui::End();
    process_mode_toggle(toggle_view, toggle_node, toggle_edge, toggle_label);
  }

  inline void EditorFrame::draw_label_specific_toolbar(const WindowInfo& info, GraphECS& ecs) {
    auto& registry = ecs.get_scene().get_registry();

    if (selection.empty() && temp_selection.empty()) {
      ImGui::Text("Left button - selection\nRight click - creation\nRight drag - grabbing");

      if (!selection_drag.dragging && !motion_drag.dragging) {
        if (info.right_button_down) {
          auto entity = registry.create();
          registry.emplace<LabelEntity>(entity, LabelEntity {
            .label = "Text"
          });
          registry.emplace<PositionComponent>(entity, info.mouse_local_x, info.mouse_local_y);

          selection.insert(entity);
        }
      }
    } else if (selection.size() == 1) {
      auto& label = registry.get<LabelEntity>(*selection.begin());
      const auto pos = get_label_position(registry, *selection.begin());

      ImGui::Text("LABEL x: %d, y: %d", static_cast<int>(pos.x), static_cast<int>(pos.y));

      if (registry.any_of<AttachComponent>(*selection.begin())) {
        const auto& attach = registry.get<const AttachComponent>(*selection.begin());
        std::string_view attach_target = "";

        if (registry.any_of<NodeEntity>(attach.target)) {
          attach_target = "NODE";
        } else if (registry.any_of<EdgeEntity>(attach.target)) {
          attach_target = "EDGE";
        }

        ImGui::Text(
          "Attached to %s with offset x: %d, y: %d", 
          attach_target.data(), 
          static_cast<int>(attach.offset_x),
          static_cast<int>(attach.offset_y)
        );

        if (ImGui::Button("Select attach target")) {
          selection = { attach.target };

          if (registry.any_of<NodeEntity>(attach.target)) {
            editor_mode = EditMode::Node;
          } else if (registry.any_of<EdgeEntity>(attach.target)) {
            editor_mode = EditMode::Edge;
          }
          
        }
      }

      float rgba[] = { 
        static_cast<float>(label.color.get_red()) / 255.0f,
        static_cast<float>(label.color.get_green()) / 255.0f,
        static_cast<float>(label.color.get_blue()) / 255.0f,
        static_cast<float>(label.color.get_alpha()) / 255.0f
      };

      if (ImGui::ColorEdit4("Color", rgba)) prevent_selection_dying = true;

      label.color = { 
        static_cast<uint8_t>(rgba[0] * 255.0f),
        static_cast<uint8_t>(rgba[1] * 255.0f),
        static_cast<uint8_t>(rgba[2] * 255.0f),
        static_cast<uint8_t>(rgba[3] * 255.0f)
      };

      char buf[100];
      std::strncpy(buf, label.label.data(), sizeof(buf));
      ImGui::InputText("Text", buf, sizeof(buf));

      if (strcmp(label.label.c_str(), buf) != 0) {
        label.label = buf;
      }
    }

    if (!selection.empty()) {
      if (ImGui::Button("Detach")) {
        for (const auto entity : selection) {
          if (registry.any_of<AttachComponent>(entity)) {
            const auto pos = get_attach_label_position(registry, entity);
            registry.emplace<PositionComponent>(entity, pos.x, pos.y);
            registry.remove<AttachComponent>(entity);
          }
        }
      }
    }
  }

  inline void EditorFrame::draw_edge_specific_toolbar(const WindowInfo& info, GraphECS& ecs) {
    auto& registry = ecs.get_scene().get_registry();

    if (selection.empty() && temp_selection.empty()) {
      ImGui::Text("Left button - selection");
    } else {
      if (ImGui::Button("<    Add arrow")) {
        for (auto& entity : selection) {
          auto& edge = registry.get<EdgeEntity>(entity);
          edge.arrow_on_start = true;
        }
      }

      ImGui::SameLine();
      if (ImGui::Button("Add arrow    >")) {
        for (auto& entity : selection) {
          auto& edge = registry.get<EdgeEntity>(entity);
          edge.arrow_on_end = true;
        }
      }

      ImGui::SameLine();
      if (ImGui::Button("Both <>##add_both")) {
        for (auto& entity : selection) {
          auto& edge = registry.get<EdgeEntity>(entity);
          edge.arrow_on_start = true;
          edge.arrow_on_end = true;
        }
      }

      if (ImGui::Button("< Remove arrow")) {
        for (auto& entity : selection) {
          auto& edge = registry.get<EdgeEntity>(entity);
          edge.arrow_on_start = false;

          if (edge.node_start == edge.node_end) edge.arrow_on_end = false;
        }
      }

      ImGui::SameLine();
      if (ImGui::Button("Remove arrow >")) {
        for (auto& entity : selection) {
          auto& edge = registry.get<EdgeEntity>(entity);
          edge.arrow_on_end = false;

          if (edge.node_start == edge.node_end) edge.arrow_on_start = false;
        }
      }

      ImGui::SameLine();
      if (ImGui::Button("Both <>##remove_both")) {
        for (auto entity : selection) {
          auto& edge = registry.get<EdgeEntity>(entity);
          edge.arrow_on_start = false;
          edge.arrow_on_end = false;
        }
      }

      if (ImGui::Button("Divide")) {
        Batch batch {};
        for (auto entity : selection) {
          const auto& edge = registry.get<const EdgeEntity>(entity);
          const auto& pos_a = registry.get<const PositionComponent>(edge.node_start);

          if (edge.node_start == edge.node_end) {
            const auto& node = registry.get<const NodeEntity>(edge.node_start);
            auto calc_params = calc_self_loop_params({ pos_a.x, pos_a.y }, node.radius);
            auto curve_middle = calculate_bezier_cubic_dot({ pos_a.x, pos_a.y }, { pos_a.x, pos_a.y }, calc_params.control_point1, calc_params.control_point2, 2.0f, 1.0f);

            auto created_node = batch.add_node({
              .position = { curve_middle.x, curve_middle.y }
            });
            batch.add_edge({
              .node_start = edge.node_start,
              .node_end = created_node,
              .arrow_on_start = edge.arrow_on_start || edge.arrow_on_end,
              .arrow_on_end = edge.arrow_on_start || edge.arrow_on_end,
            });
          } else {
            const auto& pos_b = registry.get<const PositionComponent>(edge.node_end);

            auto node = batch.add_node({
              .position = { (pos_a.x + pos_b.x) * 0.5f, (pos_a.y + pos_b.y) * 0.5f }
            });

            batch.add_edge({
              .node_start = edge.node_start,
              .node_end = node,
              .arrow_on_start = edge.arrow_on_start,
              .curve_type = edge.curve_type
            });
            batch.add_edge({
              .node_start = node,
              .node_end = edge.node_end,
              .arrow_on_end = edge.arrow_on_end,
              .curve_type = edge.curve_type
            });
          }

          registry.destroy(entity);
        }

        selection.clear();
        ecs.get_scene().append(std::move(batch));
      }

      ImGui::SameLine();
      if (ImGui::Button("Reverse")) {
        for (auto entity : selection) {
          auto& edge = registry.get<EdgeEntity>(entity);
          if (edge.node_start == edge.node_end) continue;

          std::swap(edge.node_start, edge.node_end);
        }
      }

      if (ImGui::Button("Select labels")) {
        std::unordered_set<entt::entity> new_selection;
        for (const auto& [entity, label, attach] : registry.view<const LabelEntity, const AttachComponent>().each()) {
          if (selection.contains(attach.target)) {
            new_selection.insert(entity);
          }
        }

        selection = new_selection;
        editor_mode = EditMode::Label;
      }
    }

    if (selection.size() == 1) {
      auto& edge = registry.get<EdgeEntity>(*selection.begin());

      float rgba[] = { 
        static_cast<float>(edge.color.get_red()) / 255.0f,
        static_cast<float>(edge.color.get_green()) / 255.0f,
        static_cast<float>(edge.color.get_blue()) / 255.0f,
        static_cast<float>(edge.color.get_alpha()) / 255.0f
      };

      if (ImGui::ColorEdit4("Color", rgba)) prevent_selection_dying = true;

      edge.color = { 
        static_cast<uint8_t>(rgba[0] * 255.0f),
        static_cast<uint8_t>(rgba[1] * 255.0f),
        static_cast<uint8_t>(rgba[2] * 255.0f),
        static_cast<uint8_t>(rgba[3] * 255.0f)
      };

      if (edge.node_start != edge.node_end) {
        const char* types[] = { "Linear", "Step", "Ease" };
        int current_curve_type;
        switch (edge.curve_type) {
        case EdgeCurveType::Linear: current_curve_type = 0; break;
        case EdgeCurveType::Step: current_curve_type = 1; break;
        case EdgeCurveType::Ease: current_curve_type = 2; break;
        };

        ImGui::Combo("Curve type", &current_curve_type, types, 3);

        switch (current_curve_type) {
        case 0: edge.curve_type = EdgeCurveType::Linear; break;
        case 1: edge.curve_type = EdgeCurveType::Step; break;
        case 2: edge.curve_type = EdgeCurveType::Ease; break;
        };
      }

      if (ImGui::Button("Create label")) {
        auto entity = registry.create();
        registry.emplace<LabelEntity>(entity, LabelEntity {
          .label = "Text"
        });
        registry.emplace<AttachComponent>(entity, *selection.begin(), 0.0f, 0.0f);

        selection = { entity };
        editor_mode = EditMode::Label;
      }
    }
  }

  inline void EditorFrame::draw_node_specific_toolbar(const WindowInfo& info, GraphECS& ecs) {
    auto& registry = ecs.get_scene().get_registry();

    if (selection.empty() && temp_selection.empty()) {
      ImGui::Text("Left button - selection\nRight click - creation\nRight drag - grabbing");

      if (!selection_drag.dragging && !motion_drag.dragging) {
        if (info.right_button_down) {
          auto entity = registry.create();
          registry.emplace<NodeEntity>(entity, NodeEntity {});
          registry.emplace<PositionComponent>(entity, info.mouse_local_x, info.mouse_local_y);

          selection.insert(entity);
        }
      }
    } else if (selection.size() == 1) {
      auto [node, pos] = registry.get<NodeEntity, const PositionComponent>(*selection.begin());

      ImGui::Text("NODE x: %d, y: %d", static_cast<int>(pos.x), static_cast<int>(pos.y));

      float rgba[] = { 
        static_cast<float>(node.color.get_red()) / 255.0f,
        static_cast<float>(node.color.get_green()) / 255.0f,
        static_cast<float>(node.color.get_blue()) / 255.0f,
        static_cast<float>(node.color.get_alpha()) / 255.0f
      };

      if (ImGui::ColorEdit4("Color", rgba)) prevent_selection_dying = true;

      node.color = { 
        static_cast<uint8_t>(rgba[0] * 255.0f),
        static_cast<uint8_t>(rgba[1] * 255.0f),
        static_cast<uint8_t>(rgba[2] * 255.0f),
        static_cast<uint8_t>(rgba[3] * 255.0f)
      };

      ImGui::SliderFloat("Radius", &node.radius, 5.0f, 100.0f);

      auto self_loop = get_node_connection(registry, *selection.begin(), *selection.begin());
      if (self_loop.has_value()) {
        if (ImGui::Button("Remove self-loop")) {
          registry.destroy(self_loop.value());
        }
      } else {
        if (ImGui::Button("Add self-loop")) {
          auto entity = registry.create();
          registry.emplace<EdgeEntity>(entity, EdgeEntity {
            .node_start = *selection.begin(),
            .node_end = *selection.begin(),
          });
        }
      }

      if (ImGui::Button("Create label")) {
        auto entity = registry.create();
        registry.emplace<LabelEntity>(entity, LabelEntity {
          .label = "Text"
        });
        registry.emplace<AttachComponent>(entity, *selection.begin(), 0.0f, 0.0f);

        selection = { entity };
        editor_mode = EditMode::Label;
      }
    } else if (selection.size() == 2) {
      ImGui::SameLine();
      auto edge_connection = get_node_connection(registry, *selection.begin(), *std::next(selection.begin()));
      if (edge_connection.has_value()) {
        if (ImGui::Button("Disconnect")) {
          registry.destroy(edge_connection.value());
        }
        if (ImGui::Button("Select edge between")) {
          editor_mode = EditMode::Edge;
          selection.clear();
          selection.insert(edge_connection.value());
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

    if (!selection.empty()) {
      if (ImGui::Button("Select connected edges")) {
        std::unordered_set<entt::entity> edges;

        for (const auto& [entity, edge] : registry.view<const EdgeEntity>().each()) {
          if (selection.contains(edge.node_start) || selection.contains(edge.node_end)) {
            edges.insert(entity);
          }
        }

        selection = std::move(edges);
        editor_mode = EditMode::Edge;
      }

      if (ImGui::Button("Select labels")) {
        std::unordered_set<entt::entity> new_selection;
        for (const auto& [entity, label, attach] : registry.view<const LabelEntity, const AttachComponent>().each()) {
          if (selection.contains(attach.target)) {
            new_selection.insert(entity);
          }
        }

        selection = new_selection;
        editor_mode = EditMode::Label;
      }
    }
  }

  inline void EditorFrame::draw_mode_toolbar(bool& toggle_view, bool& toggle_node, bool& toggle_edge, bool& toggle_label, bool current_view, bool current_node, bool current_edge, bool current_label) {
    ResourceCounter<IconTextures> icons_resource {};
    const auto& resource = icons_resource.get();

    toggle_view = draw_toggle_icon_button("editor_view_icon", static_cast<ImTextureID>(resource.view_icon.id), ImVec2(24.0f, 24.0f), current_view) && !current_view;
    ImGui::SameLine();
    toggle_node = draw_toggle_icon_button("editor_node_icon", static_cast<ImTextureID>(resource.node_icon.id), ImVec2(24.0f, 24.0f), current_node) && !current_node;
    ImGui::SameLine();
    toggle_edge = draw_toggle_icon_button("editor_edge_icon", static_cast<ImTextureID>(resource.edge_icon.id), ImVec2(24.0f, 24.0f), current_edge) && !current_edge;
    ImGui::SameLine();
    toggle_label = draw_toggle_icon_button("editor_label_icon", static_cast<ImTextureID>(resource.label_icon.id), ImVec2(24.0f, 24.0f), current_label) && !current_label;

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
      selection_drag.dragging = false;
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

  inline void EditorFrame::process_selection_node(const WindowInfo& info, GraphECS& ecs, float sel_left, float sel_top, float sel_right, float sel_bottom) {
    auto& registry = ecs.get_scene().get_registry();

    for (const auto& [entity, node, pos] : registry.view<const NodeEntity, const PositionComponent>().each()) {
      if (check_rect_a_in_rect_b(
        { pos.x - node.radius, pos.y - node.radius },
        { pos.x + node.radius, pos.y + node.radius },
        { sel_left, sel_top },
        { sel_right, sel_bottom }
      )) {
        if (!selection.contains(entity)) {
          temp_selection.insert(entity);
        }
      }
    }
  }

  inline void EditorFrame::process_selection_edge(const WindowInfo& info, GraphECS& ecs, float sel_left, float sel_top, float sel_right, float sel_bottom) {
    auto& registry = ecs.get_scene().get_registry();

    Vector2 corner_top_left_expanded { sel_left, sel_top };
    Vector2 corner_bottom_right_expanded { sel_right, sel_bottom };
    if ((sel_right - sel_left) * (sel_bottom - sel_top) < 5.0f) {
      corner_top_left_expanded = { corner_top_left_expanded.x - 5.0f, corner_top_left_expanded.y - 5.0f };
      corner_bottom_right_expanded = { corner_bottom_right_expanded.x + 5.0f, corner_bottom_right_expanded.y + 5.0f };
    }

    for (const auto& [entity, edge] : registry.view<const EdgeEntity>().each()) {
      auto pos_a = registry.get<const PositionComponent>(edge.node_start);

      if (edge.node_start == edge.node_end) {
        auto& node = registry.get<const NodeEntity>(edge.node_start);

        auto calc_params = calc_self_loop_params({ pos_a.x, pos_a.y }, node.radius);

        uint8_t parts = 4;
        for (uint8_t part = 0; part < parts; ++part) {
          if (approximately_check_bezier_line_in_rect([&pos_a, &calc_params, parts, part](ApproximatelySplineCallbackParams a) {
            return calculate_bezier_cubic_dot({ pos_a.x, pos_a.y }, { pos_a.x, pos_a.y }, calc_params.control_point1, calc_params.control_point2, a.divisions * static_cast<float>(parts), part * a.divisions + a.index);
          }, corner_top_left_expanded, corner_bottom_right_expanded)) {
            selection.insert(entity);
            break;
          }
        }
      } else {
        auto pos_b = registry.get<const PositionComponent>(edge.node_end);

        if (!selection.contains(entity)) {
          bool in_selection { false };

          switch(edge.curve_type) {
          case EdgeCurveType::Linear:
            in_selection = check_rect_collision_line(
              { pos_a.x, pos_a.y }, { pos_b.x, pos_b.y }, 
              corner_top_left_expanded, corner_bottom_right_expanded
            );
            break;

          case EdgeCurveType::Ease:
            in_selection = approximately_check_bezier_line_in_rect([pos_a, pos_b](ApproximatelySplineCallbackParams a) {
              return calculate_bezier_cubic_in_out_dot({ pos_a.x, pos_a.y }, { pos_b.x, pos_b.y }, a.divisions, a.index);
            }, corner_top_left_expanded, corner_bottom_right_expanded);
            break;

          case EdgeCurveType::Step:
            float mid_x = pos_a.x + (pos_b.x - pos_a.x) / 2.0f;
            in_selection = (
               check_rect_collision_line({ pos_a.x, pos_a.y }, { mid_x, pos_a.y }, corner_top_left_expanded, corner_bottom_right_expanded) ||
               check_rect_collision_line({ mid_x, pos_a.y }, { mid_x, pos_b.y }, corner_top_left_expanded, corner_bottom_right_expanded) ||
               check_rect_collision_line({ mid_x, pos_b.y }, { pos_b.x, pos_b.y }, corner_top_left_expanded, corner_bottom_right_expanded)
            );

            break;
          }

          if (in_selection) temp_selection.insert(entity);
        }
      }
    }
  }

  inline void EditorFrame::process_selection_label(const WindowInfo& info, GraphECS& ecs, float sel_left, float sel_top, float sel_right, float sel_bottom) {
    auto& registry = ecs.get_scene().get_registry();
    ResourceCounter<FontResource> font_res {};
    Font font = font_res.get().open_sans;

    for (const auto& [entity, label] : registry.view<const LabelEntity>().each()) {
      const auto label_position = get_label_position(registry, entity);
      const auto label_size = get_label_size(label);

      if (check_rect_a_in_rect_b(
        { label_position.x - label_size.x * 0.5f, label_position.y - label_size.y * 0.5f },
        { label_position.x + label_size.x * 0.5f, label_position.y + label_size.y * 0.5f },
        { sel_left, sel_top },
        { sel_right, sel_bottom }
      )) {
        if (!selection.contains(entity)) {
          temp_selection.insert(entity);
        }
      }
    }
  }

  inline void EditorFrame::process_selection(const WindowInfo& info, GraphECS& ecs, bool current_node, bool current_edge, bool current_label) {
    auto& registry = ecs.get_scene().get_registry();

    if (info.left_button_down && !ImGui::GetIO().WantCaptureMouse && !motion_drag.dragging) {
      // Selection states

      if (selection_drag.dragging) {
        selection_drag.end_x = info.mouse_local_x;
        selection_drag.end_y = info.mouse_local_y;
      } else {
        selection_drag.dragging = true;
        selection_drag.start_x = info.mouse_local_x;
        selection_drag.start_y = info.mouse_local_y;
        selection_drag.end_x = info.mouse_local_x;
        selection_drag.end_y = info.mouse_local_y;
      }

      Vector2 corner_top_left = { std::min(selection_drag.start_x, selection_drag.end_x), std::min(selection_drag.start_y, selection_drag.end_y) };
      Vector2 corner_bottom_right = { std::max(selection_drag.start_x, selection_drag.end_x), std::max(selection_drag.start_y, selection_drag.end_y) };

      // Draw selection box

      Vector2 translated_corner = translate_world_to_screen_coordinates(corner_top_left, { info.camera_x, info.camera_y }, info.camera_zoom, { static_cast<float>(info.width), static_cast<float>(info.height) });
      Vector2 translated_size = { (corner_bottom_right.x - corner_top_left.x) * info.camera_zoom, (corner_bottom_right.y - corner_top_left.y) * info.camera_zoom };

      DrawRectangleV(
        translated_corner,
        translated_size,
        { 50, 50, 255, 50 }
      );

      DrawRectangleLinesEx(Rectangle { translated_corner.x, translated_corner.y, translated_size.x, translated_size.y }, 1.0f, { 0, 0, 180, 100 });

      // Update items selection

      if (IsKeyUp(KEY_LEFT_CONTROL) && IsKeyUp(KEY_LEFT_SHIFT)) {
        selection.clear();
      }
      temp_selection.clear();

      if (current_node) process_selection_node(info, ecs, corner_top_left.x, corner_top_left.y, corner_bottom_right.x, corner_bottom_right.y);
      else if (current_edge) process_selection_edge(info, ecs, corner_top_left.x, corner_top_left.y, corner_bottom_right.x, corner_bottom_right.y);
      else if (current_label) process_selection_label(info, ecs, corner_top_left.x, corner_top_left.y, corner_bottom_right.x, corner_bottom_right.y);
    } else {
      for (auto& temp_select : temp_selection) {
        selection.insert(temp_select);
      }
      temp_selection.clear();
      selection_drag.dragging = false;
    }

    // Draw items selection

    if (prevent_selection_dying) {
      prevent_selection_dying = false;
      return;
    }

    BeginMode2D(get_camera_2d(info));

    Color selection_color = { 0, 0, 180, 100 };
    ResourceCounter<FontResource> font_res {};
    Font font = font_res.get().open_sans;

    for (auto* selection_list : { &selection, &temp_selection }) {
      for (auto selected : *selection_list) {
        if (!registry.valid(selected)) continue;

        if (current_node) {
          const auto& [node, pos] = registry.get<const NodeEntity, const PositionComponent>(selected);

          DrawCircleV(
            { pos.x, pos.y }, 
            node.radius,
            selection_color
          );
        } else if (current_edge) {
          const auto& edge = registry.get<const EdgeEntity>(selected);
          const auto& pos_a = registry.get<const PositionComponent>(edge.node_start);
          float thickness = EDGES_THICKNESS * 2.0f;

          if (edge.node_start == edge.node_end) {
            auto node = registry.get<const NodeEntity>(edge.node_start);

            Painter::draw_self_loop(
              { pos_a.x, pos_a.y },
              selection_color,
              node.radius,
              thickness
            );
          } else {
            const auto& pos_b = registry.get<const PositionComponent>(edge.node_end);

            Painter::draw_edge(
              { pos_a.x, pos_a.y },
              { pos_b.x, pos_b.y },
              selection_color,
              edge.curve_type,
              thickness
            );
          }
        } else if (current_label) {
          const auto label_position = get_label_position(registry, selected);
          const auto label_size = get_label_size(registry.get<const LabelEntity>(selected));

          const Vector2 label_corner = { label_position.x - label_size.x * 0.5f, label_position.y - label_size.y * 0.5f };

          if (registry.any_of<AttachComponent>(selected)) {
            const auto& attach = registry.get<const AttachComponent>(selected);
            DrawLineDashed(
              { label_position.x - attach.offset_x, label_position.y - attach.offset_y }, 
              label_position, 
              5, 5, { 0, 0, 0, 255 }
            );
          }

          DrawRectangleV(
            label_corner,
            label_size,
            selection_color
          );
        }
      }
    }

    EndMode2D();
  }

  inline void EditorFrame::process_motion(const WindowInfo& info, GraphECS& ecs, bool current_node, bool current_edge, bool current_label) {
    if (selection.empty() || selection_drag.dragging || current_edge || selection.empty()) {
      motion_drag.dragging = false;
      return;
    }

    auto& registry = ecs.get_scene().get_registry();

    if (info.right_button_down) {
      if (motion_drag.dragging) {
        for (auto select_entity : selection) {
          if (!registry.valid(select_entity)) continue;

          if (current_label) {
            if (registry.any_of<AttachComponent>(select_entity)) {
              auto& attach = registry.get<AttachComponent>(select_entity);
              attach.offset_x += info.mouse_local_x - motion_drag.end_x;
              attach.offset_y += info.mouse_local_y - motion_drag.end_y;
            } else {
              auto& pos = registry.get<PositionComponent>(select_entity);
              pos.x += info.mouse_local_x - motion_drag.end_x;
              pos.y += info.mouse_local_y - motion_drag.end_y;
            }
          } else {
            auto& pos = registry.get<PositionComponent>(select_entity);
            pos.x += info.mouse_local_x - motion_drag.end_x;
            pos.y += info.mouse_local_y - motion_drag.end_y;

            if (registry.all_of<VelocityComponent>(select_entity)) {
              auto& vel = registry.get<VelocityComponent>(select_entity);
              vel.x = 0.0f;
              vel.y = 0.0f;
            }
          }
        }

        motion_drag.end_x = info.mouse_local_x;
        motion_drag.end_y = info.mouse_local_y;
      } else {
        motion_drag.dragging = true;
        motion_drag.start_x = info.mouse_local_x;
        motion_drag.start_y = info.mouse_local_y;
        motion_drag.end_x = info.mouse_local_x;
        motion_drag.end_y = info.mouse_local_y;
      }
    } else {
      motion_drag.dragging = false;
    }
  }

}
