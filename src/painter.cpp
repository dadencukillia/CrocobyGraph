#include "painter.hpp"
#include "color.hpp"
#include "entities.hpp"
#include "math.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "resources/memory.hpp"

namespace CrocobyGraph {

  void Painter::load() {
    open_sans_font = LoadFontFromMemory(".ttf", OpenSansFontData, OpenSansFontSize, 96, nullptr, 0);
    SetTextureFilter(open_sans_font.texture, TEXTURE_FILTER_BILINEAR);
  }

  Painter::~Painter() {
    UnloadFont(open_sans_font);
  }

  void Painter::draw_node(Vector2 pos, Color color, double radius) {
    DrawCircle(pos.x, pos.y, static_cast<float>(radius), color);
  }

  void Painter::draw_edge(Vector2 from, Vector2 to, Color color, EdgeCurveType curve) {
    float thickness = 3.0f;

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

  void Painter::draw_self_loop(Vector2 pos, Color color, double radius) {
    float thickness = 3.0f;
    float angle = std::atan2(pos.y, pos.x);
    float width = 30.0f * PI / 180.0f;
    float length = radius * 4.0f;
    Vector2 p1 = { pos.x + std::cos(angle - width) * length, pos.y + std::sin(angle - width) * length };
    Vector2 p2 = { pos.x + std::cos(angle + width) * length, pos.y + std::sin(angle + width) * length };

    const Vector2 points[] = { pos, p1, p2, pos };
    DrawSplineBezierCubic(points, 4, thickness, color);
  }

  void Painter::draw_label(Vector2 pos, std::string_view text, Color color) {
    float font_size = 18.0f;
    float spacing = 0.0f;

    auto dimension = MeasureTextEx(open_sans_font, text.data(), font_size, spacing);
    DrawTextEx(open_sans_font, text.data(), { pos.x - dimension.x / 2.0f, pos.y - dimension.y / 2.0f }, font_size, spacing, color);
  }

  void Painter::draw_arrow(Vector2 from, Vector2 to, float radius, Color color, EdgeCurveType curve) {
    if (from.x == to.x && from.y == to.y) {
      uint32_t divisions = 64;
      float angle = std::atan2(from.y, from.x);
      float width = 30 * PI / 180.0f;
      float length = radius * 4.0f;
      Vector2 p1 = { from.x + std::cos(angle - width) * length, from.y + std::sin(angle - width) * length };
      Vector2 p2 = { from.x + std::cos(angle + width) * length, from.y + std::sin(angle + width) * length };

      auto dot_by_index = [&](uint32_t index) {
        return calculate_bezier_cubic_dot(from, to, p1, p2, divisions, static_cast<float>(index));
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

    DrawLineEx(pos, first, 3.0f, color);
    DrawLineEx(pos, second, 3.0f, color);
  }

}
