#include "labels.hpp"
#include "../config.hpp"
#include "calc_impls.hpp"
#include "entt/entt.hpp"
#include "math.hpp"
#include "raylib.h"
#include "resource_counter.hpp"
#include "resources.hpp"
#include <cassert>

namespace CrocobyGraph {

  Vector2 get_label_position(const entt::registry& registry, entt::entity label_entity) {
    if (registry.any_of<AttachComponent>(label_entity)) {
      return get_attach_label_position(registry, label_entity);
    } else {
      return get_free_label_position(registry, label_entity);
    }
  }

  Vector2 get_label_size(const LabelEntity& label_entity) {
    ResourceCounter<FontResource> font_res {};
    const Font font { font_res.get().open_sans };

    return MeasureTextEx(font, label_entity.label.data(), LABEL_FONT_SIZE, LABEL_SPACING);
  }

  Vector2 get_free_label_position(const entt::registry& registry, entt::entity free_label_entity) {
    const auto& pos_component { registry.get<const PositionComponent>(free_label_entity) };
    return { pos_component.x, pos_component.y };
  }

  Vector2 get_attach_label_position(const entt::registry& registry, entt::entity attach_label_entity) {
    const auto& attachment { registry.get<const AttachComponent>(attach_label_entity) };
    const auto target { attachment.target };
    Vector2 pivot_point {};

    if (registry.all_of<NodeEntity, PositionComponent>(target)) {
      pivot_point = get_node_pivot_point_label(registry.get<PositionComponent>(target));
    } else if (registry.any_of<EdgeEntity>(target)) {
      pivot_point = get_edge_pivot_point_label(registry, registry.get<EdgeEntity>(target));
    } else assert(false && "Label must be attached to Node or Edge");

    return {
      attachment.offset_x + pivot_point.x,
      attachment.offset_y + pivot_point.y,
    };
  }

  Vector2 get_node_pivot_point_label(const PositionComponent& node_position_component) {
    return { node_position_component.x, node_position_component.y };
  }

  Vector2 get_edge_pivot_point_label(const entt::registry& registry, const EdgeEntity& edge_entity) {
    const auto& pos_a { registry.get<const PositionComponent>(edge_entity.node_start) };

    if (edge_entity.node_start == edge_entity.node_end) {
      const auto& node { registry.get<const NodeEntity>(edge_entity.node_start) };
      const auto calc_params { calc_self_loop_params({ pos_a.x, pos_a.y }, node.radius) };

      return calculate_bezier_cubic_dot({ pos_a.x, pos_a.y }, { pos_a.x, pos_a.y }, calc_params.control_point1, calc_params.control_point2, 2.0f, 1.0f);
    } else {
      const auto& pos_b { registry.get<const PositionComponent>(edge_entity.node_end) };

      return {
        (pos_a.x + pos_b.x) * 0.5f,
        (pos_a.y + pos_b.y) * 0.5f,
      };
    }
  }

}
