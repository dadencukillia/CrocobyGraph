#include "painter.hpp"
#include "../interface/color.hpp"
#include "../interface/entities.hpp"
#include "../resources/open_sans.hpp"
#include "math.hpp"
#include "calc_impls.hpp"
#include "physics.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>

namespace CrocobyGraph {

  struct FontResource {
    Font open_sans;

    FontResource() {
      open_sans = LoadFontFromMemory(".ttf", OpenSansFontData, OpenSansFontSize, 96, nullptr, 0);
      SetTextureFilter(open_sans.texture, TEXTURE_FILTER_BILINEAR);
      std::cout << "Open Sans font loaded\n";
    }

    ~FontResource() {
      UnloadFont(open_sans);
      std::cout << "Open Sans font unloaded\n";
    }
  };

  Painter::Painter() = default;
  Painter::~Painter() = default;

  void Painter::draw_jelly_node(const std::vector<PositionComponent>& points, Vector2 center_pos, Color color) {
    Vector2 fan_points[jelly_points + 2] = {};
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
      float mid_x = from.x + (to.x - from.x) / 2.0f;
      DrawLineEx(from, { mid_x, from.y }, thickness, color);
      DrawLineEx({ mid_x, from.y }, { mid_x, to.y }, thickness, color);
      DrawLineEx({ mid_x, to.y }, to, thickness, color);
      break;
    }
  }

  void Painter::draw_self_loop(Vector2 pos, Color color, float radius, float thickness) {
    auto calc_params = calc_self_loop_params(pos, radius);
    DrawSplineSegmentBezierCubic(pos, calc_params.control_point1, calc_params.control_point2, pos, thickness, color);
  }

  void Painter::draw_label(Vector2 pos, std::string_view text, Color color) {
    ResourceCounter<FontResource> font_resource {};
    const auto& resource = font_resource.get();

    float font_size = 18.0f;
    float spacing = 0.0f;

    auto dimension = MeasureTextEx(resource.open_sans, text.data(), font_size, spacing);
    DrawTextEx(resource.open_sans, text.data(), { pos.x - dimension.x / 2.0f, pos.y - dimension.y / 2.0f }, font_size, spacing, color);
  }

  void Painter::draw_arrow(Vector2 from, Vector2 to, float radius, Color color, EdgeCurveType curve, float thickness) {
    if (from.x == to.x && from.y == to.y) {
      uint32_t divisions = 64;
      auto calc_params = calc_self_loop_params(from, radius);

      auto dot_by_index = [&](uint32_t index) {
        return calculate_bezier_cubic_dot(from, to, calc_params.control_point1, calc_params.control_point2, divisions, static_cast<float>(index));
      };

      auto result = approximately_circle_intersection(divisions, radius, to, dot_by_index, divisions / 2);

      from = dot_by_index(std::max(1u, result.intersection_point_index) - 1);
      to = result.intersection_point;
      radius -= result.distance;
    } else if (curve == EdgeCurveType::Ease) {
      uint32_t divisions = 128;

      auto dot_by_index = [&](uint32_t index) {
        return calculate_bezier_cubic_in_out_dot(from, to, divisions, static_cast<float>(index));
      };

      auto result = approximately_circle_intersection(divisions, radius, to, dot_by_index);

      from = dot_by_index(std::max(1u, result.intersection_point_index) - 1);
      to = result.intersection_point;
      radius -= result.distance;
    } else if (curve == EdgeCurveType::Step) {
      if (to.x != from.x) {
        float mid_x = from.x + (to.x - from.x) / 2.0f;
        from = { mid_x, to.y };
      }
    }

    Vector2 vector = { to.x - from.x, to.y - from.y };
    float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);
    Vector2 normalized = { vector.x / length, vector.y / length };
    Vector2 pos = { to.x - normalized.x * radius, to.y - normalized.y * radius };
    float angle = std::atan2(-normalized.y, -normalized.x);
    float sharpness = 30.0 * PI / 180.0;
    float len = 10.0;

    Vector2 first = { pos.x + static_cast<float>(cos(angle + sharpness)) * len, pos.y + static_cast<float>(sin(angle + sharpness)) * len };
    Vector2 second = { pos.x + static_cast<float>(cos(angle - sharpness)) * len, pos.y + static_cast<float>(sin(angle - sharpness)) * len };

    DrawLineEx(pos, first, thickness, color);
    DrawLineEx(pos, second, thickness, color);
  }

}
