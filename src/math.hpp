#ifndef _CGRAPH_MATH_HPP_
#define _CGRAPH_MATH_HPP_

#include "raylib.h"
#include <cstdint>
#include <functional>

namespace CrocobyGraph {

  struct ApproximatelyIntersectResult {
    Vector2 intersection_point { 0 };
    uint32_t intersection_point_index { 0 };
    float distance { 0.0f };
  };

  float raylib_ease_cubic_in_out(float t, float b, float c, float d);
  Vector2 calculate_bezier_cubic_dot(Vector2 start, Vector2 end, Vector2 c1, Vector2 c2, float divisions, float index);
  Vector2 calculate_bezier_cubic_in_out_dot(Vector2 a, Vector2 b, float divisions, float index);
  ApproximatelyIntersectResult approximately_circle_intersection(uint32_t spline_segments, float circle_radius, Vector2 circle_center, std::function<Vector2(uint32_t)> spline_dot_function, uint32_t left, uint32_t right);
  ApproximatelyIntersectResult approximately_circle_intersection(uint32_t spline_segments, float circle_radius, Vector2 circle_center, std::function<Vector2(uint32_t)> spline_dot_function, uint32_t left = 0);

}

#endif
