#ifndef _CGRAPH_INTERNAL_CALC_IMPLS_HPP_
#define _CGRAPH_INTERNAL_CALC_IMPLS_HPP_

#include <utility>
#include "raylib.h"

namespace CrocobyGraph {

  struct SelfLoopParams {
    float width {};
    float length {};
    Vector2 axis {};
    Vector2 control_point1 {};
    Vector2 control_point2 {};
  };

  [[nodiscard]] SelfLoopParams calc_self_loop_params(Vector2 node_pos, float radius) noexcept;

  [[nodiscard]] inline std::pair<Vector2, Vector2> calc_step_curve_middle_points(Vector2 curve_start, Vector2 curve_end) {
    float mid_x = curve_start.x + (curve_end.x - curve_start.x) * 0.5f;
    return { { mid_x, curve_start.y }, { mid_x, curve_end.y } };
  }

}

#endif
