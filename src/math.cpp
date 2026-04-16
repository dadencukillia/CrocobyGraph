#include "math.hpp"
#include "raylib.h"
#include <cmath>
#include <cstdint>

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

}
