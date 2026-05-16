#include "painter.hpp"
#include "../interface/color.hpp"
#include "../interface/entities.hpp"
#include "math.hpp"
#include "calc_impls.hpp"
#include "physics.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "raylib.h"

namespace CrocobyGraph {
  Painter::Painter() = default;
  Painter::~Painter() = default;

  void Painter::draw_jelly_node(const std::vector<PositionComponent>& points, Vector2 center_pos, Color color) {
    Vector2 fan_points[jelly_points + 2];
    fan_points[0] = { center_pos.x, center_pos.y };
    fan_points[jelly_points + 1] = { points[jelly_points - 1].x, points[jelly_points - 1].y };
    for (size_t i = 0; i < jelly_points; ++i) {
      fan_points[i + 1] = { points[jelly_points - 1 - i].x, points[jelly_points - 1 - i].y };
    }

    DrawTriangleFan(fan_points, jelly_points + 2, color);
  }

  void Painter::draw_node(Vector2 pos, Color color, float radius) {
    DrawCircleV(pos, radius, color);
  }

  void Painter::draw_edge(Vector2 from, Vector2 to, Color color, EdgeCurveType curve, float thickness) {
    switch (curve) {
    case EdgeCurveType::Linear:
      DrawLineEx(from, to, thickness, color);
      break;

    case EdgeCurveType::Ease:
      DrawLineBezier(from, to, thickness, color);
      break;

    case EdgeCurveType::Step:
      const auto middle_points { calc_step_curve_middle_points(from, to) };
      DrawLineEx(from, middle_points.first, thickness, color);
      DrawLineEx(middle_points.first, middle_points.second, thickness, color);
      DrawLineEx(middle_points.second, to, thickness, color);
      break;
    }
  }

  void Painter::draw_self_loop(Vector2 pos, Color color, float radius, float thickness) {
    const auto calc_params { calc_self_loop_params(pos, radius) };
    DrawSplineSegmentBezierCubic(pos, calc_params.control_point1, calc_params.control_point2, pos, thickness, color);
  }

  void Painter::draw_label(Vector2 pos, std::string_view text, Color color) {
    ResourceCounter<FontResource> font_resource {};
    const auto& resource { font_resource.get() };

    const auto dimension { MeasureTextEx(resource.open_sans, text.data(), LABEL_FONT_SIZE, LABEL_SPACING) };
    DrawTextEx(resource.open_sans, text.data(), { pos.x - dimension.x * 0.5f, pos.y - dimension.y * 0.5f }, LABEL_FONT_SIZE, LABEL_SPACING, color);
  }

  void Painter::draw_arrow(Vector2 from, Vector2 to, float radius, Color color, EdgeCurveType curve, float thickness) {
    if (from.x == to.x && from.y == to.y) {
      const uint32_t segments { 64 };
      const auto calc_params { calc_self_loop_params(from, radius) };

      auto dot_by_index = [from, to, &calc_params, segments](uint32_t index) {
        return calculate_bezier_cubic_dot(from, to, calc_params.control_point1, calc_params.control_point2, segments, static_cast<float>(index));
      };

      auto result { approximately_circle_intersection(segments, radius, to, dot_by_index, segments * 0.5f) };

      from = dot_by_index(std::max(1u, result.intersection_point_index) - 1);
      to = result.intersection_point;
      radius -= result.distance;
    } else if (curve == EdgeCurveType::Ease) {
      const uint32_t segments { 128 };

      auto dot_by_index = [from, to, segments](uint32_t index) {
        return calculate_bezier_cubic_in_out_dot(from, to, segments, static_cast<float>(index));
      };

      const auto result { approximately_circle_intersection(segments, radius, to, dot_by_index) };

      from = dot_by_index(std::max(1u, result.intersection_point_index) - 1);
      to = result.intersection_point;
      radius -= result.distance;
    } else if (curve == EdgeCurveType::Step) {
      if (to.x != from.x) {
        from = calc_step_curve_middle_points(from, to).second;
      }
    }

    constexpr float sharpness { EDGE_ARROW_SHARPNESS_DEGRESS * PI / 180.0f };
    static const Vector2 rot_vec = {
      std::cos(sharpness),
      std::sin(sharpness)
    };

    const Vector2 normalized { normalize_vector({ to.x - from.x, to.y - from.y }) };
    const Vector2 line { -normalized.x * EDGE_ARROW_LENGTH, -normalized.y * EDGE_ARROW_LENGTH };
    const Vector2 pos { to.x - normalized.x * radius, to.y - normalized.y * radius };

    const Vector2 rel_a { rotate_vector(line, rot_vec) };
    const Vector2 rel_b { rotate_vector(line, { rot_vec.x, -rot_vec.y }) };

    const Vector2 a { pos.x + rel_a.x, pos.y + rel_a.y };
    const Vector2 b { pos.x + rel_b.x, pos.y + rel_b.y };

    DrawLineEx(pos, a, thickness, color);
    DrawLineEx(pos, b, thickness, color);
  }

}
