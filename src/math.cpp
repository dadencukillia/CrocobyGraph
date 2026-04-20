#include "math.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <queue>

namespace CrocobyGraph {

  float raylib_ease_cubic_in_out(float t, float b, float c, float d) {
    float result = 0.0f;

    if ((t /= 0.5f*d) < 1) result = 0.5f*c*t*t*t + b;
    else {
      t -= 2;
      result = 0.5f*c*(t*t*t + 2.0f) + b;
    }

    return result;
  }

  Vector2 calculate_bezier_cubic_dot(Vector2 start, Vector2 end, Vector2 c1, Vector2 c2, float divisions, float index) {
    const float step = 1.0f / divisions;
    float t = step * index;

    float a = powf(1.0f - t, 3);
    float b = 3.0f*powf(1.0f - t, 2)*t;
    float c = 3.0f*(1.0f - t)*powf(t, 2);
    float d = powf(t, 3);

    return {
      a * start.x + b * c1.x + c * c2.x + d * end.x,
      a * start.y + b * c1.y + c * c2.y + d * end.y,
    };
  }

  Vector2 calculate_bezier_cubic_in_out_dot(Vector2 a, Vector2 b, float divisions, float index) {
    return {
      a.x + (b.x - a.x) / divisions * index,
      raylib_ease_cubic_in_out(index, a.y, b.y - a.y, divisions),
    };
  }

  ApproximatelyIntersectResult approximately_circle_intersection(uint32_t spline_segments, float circle_radius, Vector2 circle_center, std::function<Vector2(uint32_t)> spline_dot_function, uint32_t left, uint32_t right) {
    float divisions { static_cast<float>(spline_segments) };
    float radius_square { circle_radius * circle_radius };
    Vector2 dot { 0 };
    float distance { 0.0f };
    uint32_t index { 0 };
    while (left != right) {
      uint32_t mid = left + std::floor((right - left) / 2.0f);
      index = mid;
      dot = spline_dot_function(index);
      distance = (dot.x - circle_center.x) * (dot.x - circle_center.x) + (dot.y - circle_center.y) * (dot.y - circle_center.y);
      if (distance > radius_square) {
        left = mid + 1;
      } else if (distance < radius_square) {
        right = mid;
      } else {
        left = mid;
        right = mid;
      }
    }

    return { dot, index, std::sqrt(distance) };
  }

  ApproximatelyIntersectResult approximately_circle_intersection(uint32_t spline_segments, float circle_radius, Vector2 circle_center, std::function<Vector2(uint32_t)> spline_dot_function, uint32_t left) {
    return approximately_circle_intersection(spline_segments, circle_radius, circle_center, spline_dot_function, left, spline_segments - 1);
  }

  bool check_point_in_rect(Vector2 point, Vector2 rect_top_left, Vector2 rect_bottom_right) {
    return rect_top_left.x <= point.x && rect_top_left.y <= point.y && rect_bottom_right.x >= point.x && rect_bottom_right.y >= point.y;
  }

  bool check_rect_collision_line(Vector2 line_start, Vector2 line_end, Vector2 rect_top_left, Vector2 rect_bottom_right) {
    if (check_point_in_rect(line_start, rect_top_left, rect_bottom_right)) return true;

    Vector2 t;
    if (CheckCollisionLines(line_start, line_end, rect_top_left, { rect_bottom_right.x, rect_top_left.y }, &t)) return true;
    if (CheckCollisionLines(line_start, line_end, { rect_bottom_right.x, rect_top_left.y }, rect_bottom_right, &t)) return true;
    if (CheckCollisionLines(line_start, line_end, rect_bottom_right, { rect_top_left.x, rect_bottom_right.y }, &t)) return true;
    if (CheckCollisionLines(line_start, line_end, { rect_top_left.x, rect_bottom_right.y }, rect_top_left, &t)) return true;

    return false;
  }

  bool check_rect_a_in_rect_b(Vector2 rect_a_top_left, Vector2 rect_a_bottom_right, Vector2 rect_b_top_left, Vector2 rect_b_bottom_right) {
    return rect_a_top_left.x <= rect_b_bottom_right.x && rect_a_bottom_right.x >= rect_b_top_left.x && rect_a_top_left.y <= rect_b_bottom_right.y && rect_a_bottom_right.y >= rect_b_top_left.y;
  }

  bool approximately_check_bezier_line_in_rect(std::function<Vector2(ApproximatelySplineCallbackParams)> spline_dot_function, Vector2 rect_top_left, Vector2 rect_bottom_right, float threshold) {
    std::queue<ApproximatelySplineCallbackParams> parts;
    parts.push({
      .divisions = 1,
      .index = 0
    });

    for (; !parts.empty(); parts.pop()) {
      auto& front = parts.front();

      auto a = spline_dot_function({
        .divisions = front.divisions,
        .index = front.index,
      });

      auto b = spline_dot_function({
        .divisions = front.divisions,
        .index = front.index + 1,
      });

      Vector2 top_left = {
        std::min(a.x, b.x),
        std::min(a.y, b.y),
      };

      Vector2 bottom_right = {
        std::max(a.x, b.x),
        std::max(a.y, b.y),
      };

      if (check_rect_a_in_rect_b(top_left, bottom_right, rect_top_left, rect_bottom_right)) {
        if ((bottom_right.x - top_left.x) * (bottom_right.y - top_left.y) < threshold) return true;

        parts.push({
          .divisions = front.divisions * 2,
          .index = front.index * 2
        });

        parts.push({
          .divisions = front.divisions * 2,
          .index = front.index * 2 + 1
        });
      }
    }

    return false;
  }

}
