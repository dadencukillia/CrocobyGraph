#ifndef _CGRAPH_MATH_HPP_
#define _CGRAPH_MATH_HPP_

#include <cmath>
#include <cstdint>
#include <functional>
#include "raylib.h"

namespace CrocobyGraph {

  struct ApproximatelyIntersectResult {
    Vector2 intersection_point { 0 };
    uint32_t intersection_point_index { 0 };
    float distance { 0.0f };
  };

  struct ApproximatelySplineCallbackParams {
    uint32_t divisions;
    uint32_t index;
  };

  [[nodiscard]] float raylib_ease_cubic_in_out(float t, float b, float c, float d);
  [[nodiscard]] Vector2 calculate_bezier_cubic_dot(Vector2 start, Vector2 end, Vector2 c1, Vector2 c2, float divisions, float index);
  [[nodiscard]] Vector2 calculate_bezier_cubic_in_out_dot(Vector2 a, Vector2 b, float divisions, float index);
  [[nodiscard]] ApproximatelyIntersectResult approximately_circle_intersection(uint32_t spline_segments, float circle_radius, Vector2 circle_center, std::function<Vector2(uint32_t)> spline_dot_function, uint32_t left, uint32_t right);
  [[nodiscard]] ApproximatelyIntersectResult approximately_circle_intersection(uint32_t spline_segments, float circle_radius, Vector2 circle_center, std::function<Vector2(uint32_t)> spline_dot_function, uint32_t left = 0);
  [[nodiscard]] bool check_point_in_rect(Vector2 point, Vector2 rect_top_left, Vector2 rect_bottom_right);
  [[nodiscard]] bool check_rect_collision_line(Vector2 line_start, Vector2 line_end, Vector2 rect_top_left, Vector2 rect_bottom_right);
  [[nodiscard]] bool check_rect_a_in_rect_b(Vector2 rect_a_top_left, Vector2 rect_a_bottom_right, Vector2 rect_b_top_left, Vector2 rect_b_bottom_right);
  [[nodiscard]] bool approximately_check_bezier_line_in_rect(std::function<Vector2(ApproximatelySplineCallbackParams)> spline_dot_function, Vector2 rect_top_left, Vector2 rect_bottom_right, float threshold = 2.0f);

  [[nodiscard]] inline Vector2 translate_world_to_screen_coordinates(Vector2 screen_pos, Vector2 camera_pos, float camera_zoom, Vector2 window_size) {
    return { (screen_pos.x - camera_pos.x) * camera_zoom + window_size.x * 0.5f, (screen_pos.y - camera_pos.y) * camera_zoom + window_size.y * 0.5f };
  }

  inline void crossplatform_sincos(float angle, float* s, float* c) noexcept {
#if defined(__GNUC__)
    sincosf(angle, s, c); 
#elif defined(__clang__)
    *s = __builtin_sinf(angle);
    *c = __builtin_cosf(angle);
#else
    *s = std::sin(angle);
    *c = std::cos(angle);
#endif
  }

  [[nodiscard]] constexpr float constexpr_sqrt(float x) noexcept {
    if (x >= 0 && x < std::numeric_limits<float>::infinity()) {
      float curr { x };
      float prev { 0 };
      while (curr != prev) {
        prev = curr;
        curr = 0.5f * (curr + x / curr);
      }
      return curr;
    }
    return std::numeric_limits<float>::quiet_NaN();
  }

  [[nodiscard]] constexpr Vector2 normalize_vector(Vector2 vec) noexcept {
    float len_sqr = vec.x * vec.x + vec.y * vec.y;
    if (len_sqr == 0) return { 1.0f, 0.0f };

    float len;
    if consteval {
      len = constexpr_sqrt(len_sqr);
    } else {
      len = std::sqrt(len_sqr);
    }

    float dist = 1.0f / len;
    return { 
      vec.x * dist,
      vec.y * dist
    };
  }

  [[nodiscard]] constexpr Vector2 rotate_vector(Vector2 vec, Vector2 normalized_rotation_vec) noexcept {
    return {
      vec.x * normalized_rotation_vec.x - vec.y * normalized_rotation_vec.y,
      vec.x * normalized_rotation_vec.y + vec.y * normalized_rotation_vec.x
    };
  }

}

#endif
