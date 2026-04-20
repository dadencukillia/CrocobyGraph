#ifndef _CGRAPH_CONFIG_HPP_
#define _CGRAPH_CONFIG_HPP_

#include "color.hpp"

namespace CrocobyGraph {

  // Privimitive colors
  inline constexpr Color DEFAULT_NODE_COLOR { 0xFFFFFFFF };
  inline constexpr Color DEFAULT_EDGE_COLOR { 0xFFFFFFFF };
  inline constexpr Color DEFAULT_LABEL_COLOR { 0x000000FF };

  // Primitive params
  inline constexpr float DEFAULT_NODE_RADIUS { 20.f };

  // Physics
  inline constexpr float PHYSICS_ATTRACTION_CONSTANT { 5.f };
  inline constexpr float PHYSICS_REPULSION_CONSTANT { 1'000'000.f };
  inline constexpr float PHYSICS_GRAVITY_CONSTANT { .8f };
  inline constexpr float PHYSICS_ATTRACTION_IDEAL_DISTANCE { 200.f };
  inline constexpr float PHYSICS_FRICTION_CONSTANT { 0.5f };

  // ECS
  inline constexpr float TARGET_TPS { 60.f };
  inline constexpr double TICKS_INTERVAL { 1. / static_cast<double>(TARGET_TPS) };

  // Render
  inline constexpr float EDGES_THICKNESS { 1.0f };
  inline constexpr float LABELS_VISIBLE_DISTANCE { 500.0f };

}

#endif
