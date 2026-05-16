#include "calc_impls.hpp"
#include "../config.hpp"
#include "math.hpp"
#include <cmath>
#include "raylib.h"

namespace CrocobyGraph {

  SelfLoopParams calc_self_loop_params(Vector2 node_pos, float radius) noexcept {
    constexpr float width { SELF_LOOP_WIDTH_DEGREES * PI / 180.0f };
    static const Vector2 rot_vec {
      std::cos(width),
      std::sin(width)
    };

    const float length { radius * SELF_LOOP_LENGTH_MULTIPLIER };
    const Vector2 axis_dir { normalize_vector(node_pos) };
    const Vector2 axis { axis_dir.x * length, axis_dir.y * length };
    const Vector2 p1_rel { rotate_vector(axis, rot_vec) };
    const Vector2 p2_rel { rotate_vector(axis, { rot_vec.x, -rot_vec.y }) };
    const Vector2 p1 { node_pos.x + p1_rel.x, node_pos.y + p1_rel.y };
    const Vector2 p2 { node_pos.x + p2_rel.x, node_pos.y + p2_rel.y };

    return {
      width, length, axis, p1, p2
    };
  }

}
