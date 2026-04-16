#include "painter.hpp"
#include "color.hpp"
#include "entities.hpp"
#include "raylib.h"
#include <cmath>
#include <cstdint>
#include "resources/memory.hpp"

namespace CrocobyGraph {

  float EaseCubicInOut(float t, float b, float c, float d) {
    float result = 0.0f;

    if ((t /= 0.5f*d) < 1) result = 0.5f*c*t*t*t + b;
    else {
      t -= 2;
      result = 0.5f*c*(t*t*t + 2.0f) + b;
    }

    return result;
  }

  Vector2 calculate_bezier_dot(Vector2 a, Vector2 b, float divisions, float index) {
    return {
      a.x + (b.x - a.x) / divisions * index,
      EaseCubicInOut(index, a.y, b.y - a.y, divisions),
    };
  }

  void Painter::load() {
    open_sans_font = LoadFontFromMemory(".ttf", OpenSansFontData, OpenSansFontSize, 48, nullptr, 0);
    SetTextureFilter(open_sans_font.texture, TEXTURE_FILTER_BILINEAR);
  }

  Painter::~Painter() {
    UnloadFont(open_sans_font);
  }

  void Painter::draw_node(Vector2 pos, Color color, double radius) {
    DrawCircle(pos.x, pos.y, static_cast<float>(radius), color);
  }

  void Painter::draw_edge(Vector2 from, Vector2 to, Color color, EdgeCurveType curve) {
    Vector2 start = { from.x, from.y };
    Vector2 end = { to.x, to.y };
    float thickness = 3.0f;

    switch (curve) {
    case EdgeCurveType::Linear:
      DrawLineEx(start, end, thickness, color);
      break;

    case EdgeCurveType::Ease:
      DrawLineBezier(start, end, thickness, color);
      break;

    case EdgeCurveType::Step:
      float mid_x = start.x + (end.x - start.x) / 2.0f;
      DrawLineEx(start, { mid_x, start.y }, thickness, color);
      DrawLineEx({ mid_x, start.y }, { mid_x, end.y }, thickness, color);
      DrawLineEx({ mid_x, end.y }, end, thickness, color);
      break;
    }
  }

  void Painter::draw_label(Vector2 pos, std::string_view text, Color color) {
    float font_size = 18.0f;
    float spacing = 0.0f;

    auto dimension = MeasureTextEx(open_sans_font, text.data(), font_size, spacing);
    DrawTextEx(open_sans_font, text.data(), { pos.x - dimension.x / 2.0f, pos.y - dimension.y / 2.0f }, font_size, spacing, color);
  }

  void Painter::draw_arrow(Vector2 from, Vector2 to, float radius, Color color, EdgeCurveType curve) {
    if (curve == EdgeCurveType::Ease) {
      float radius_square = radius * radius;

      float divisions = 128.0f;
      int32_t left = 0;
      int32_t right = divisions - 1.0f;
      Vector2 dot;
      float distance;
      while (left != right) {
        int32_t mid = left + std::floor((right - left) / 2.0f);
        dot = calculate_bezier_dot({ from.x, from.y }, { to.x, to.y }, divisions, static_cast<float>(mid));
        distance = (dot.x - to.x) * (dot.x - to.x) + (dot.y - to.y) * (dot.y - to.y);
        if (distance > radius_square) {
          left = mid + 1;
        } else if (distance < radius_square) {
          right = mid;
        } else {
          left = mid;
          right = mid;
        }
      }

      to.x = dot.x;
      to.y = dot.y;
      dot = calculate_bezier_dot({ from.x, from.y }, { to.x, to.y }, divisions, static_cast<float>(std::max(0, left - 1)));
      from.x = dot.x;
      from.y = dot.y;
      radius -= std::sqrt(distance);
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
