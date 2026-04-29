#ifndef _CGRAPH_INTERNAL_LABELS_HPP_
#define _CGRAPH_INTERNAL_LABELS_HPP_

#include "../interface/entities.hpp"
#include "../interface/components.hpp"
#include "entt/entity/fwd.hpp"
#include "raylib.h"

namespace CrocobyGraph {

  [[nodiscard]] Vector2 get_label_position(const entt::registry& registry, entt::entity);
  [[nodiscard]] Vector2 get_label_size(const LabelEntity& label_entity);
  [[nodiscard]] Vector2 get_free_label_position(const entt::registry& registry, entt::entity);
  [[nodiscard]] Vector2 get_attach_label_position(const entt::registry& registry, entt::entity);

  [[nodiscard]] Vector2 get_node_pivot_point_label(const PositionComponent& node_position_component);
  [[nodiscard]] Vector2 get_edge_pivot_point_label(const entt::registry& registry, const EdgeEntity& edge_entity);

}

#endif
