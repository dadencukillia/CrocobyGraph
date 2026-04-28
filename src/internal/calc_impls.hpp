#ifndef _CGRAPH_CALC_IMPLS_HPP_
#define _CGRAPH_CALC_IMPLS_HPP_

#include "../config.hpp"
#include "raylib.h"
#include <cmath>

namespace CrocobyGraph {

  struct SelfLoopParams {
    float angle {};
    float width {};
    float length {};
    Vector2 control_point1 {};
    Vector2 control_point2 {};
  };

  [[nodiscard]] constexpr SelfLoopParams calc_self_loop_params(Vector2 node_pos, float radius) {
    float angle = std::atan2(node_pos.y, node_pos.x);
    float width = SELF_LOOP_WIDTH_DEGREES * PI / 180.0f;
    float length = radius * SELF_LOOP_LENGTH_MULTIPLIER;
    Vector2 p1 = { node_pos.x + std::cos(angle - width) * length, node_pos.y + std::sin(angle - width) * length };
    Vector2 p2 = { node_pos.x + std::cos(angle + width) * length, node_pos.y + std::sin(angle + width) * length };

    return {
      angle, width, length, p1, p2
    };
  }

  [[nodiscard]] constexpr std::pair<Vector2, Vector2> calc_step_curve_middle_points(Vector2 curve_start, Vector2 curve_end) {
    float mid_x = curve_start.x + (curve_end.x - curve_start.x) / 2.0f;
    return { { mid_x, curve_start.y }, { mid_x, curve_end.y } };
  }

}

#endif
