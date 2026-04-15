#include "painter.hpp"
#include "raylib.h"
#include <cmath>

namespace CrocobyGraph {

  void Painter::draw_node(float x, float y, Color color, double radius) {
    DrawCircle(x, y, static_cast<float>(radius), color);
  }

  void Painter::draw_edge(float from_x, float from_y, float to_x, float to_y, Color color, EdgeCurveType curve) {
    DrawLineEx({ from_x, from_y }, { to_x, to_y }, 3.0f, color);
  }

  void Painter::draw_label(float x, float y, std::string_view text, Color color) {
  }

  void Painter::draw_arrow(float from_x, float from_y, float to_x, float to_y, float radius, Color color) {
    float vector_x = to_x - from_x;
    float vector_y = to_y - from_y;
    float length = std::sqrt(vector_x * vector_x + vector_y * vector_y);
    float normalized_x = vector_x / length;
    float normalized_y = vector_y / length;
    float pos_x = to_x - normalized_x * radius;
    float pos_y = to_y - normalized_y * radius;
    float angle = std::atan2(-normalized_y, -normalized_x);
    float sharpness = 30.0 * PI / 180.0;
    float len = 10.0;

    float first_x = pos_x + cos(angle + sharpness) * len;
    float first_y = pos_y + sin(angle + sharpness) * len;
    float second_x = pos_x + cos(angle - sharpness) * len;
    float second_y = pos_y + sin(angle - sharpness) * len;

    DrawLineEx({ pos_x, pos_y }, { first_x, first_y }, 3.0f, color);
    DrawLineEx({ pos_x, pos_y }, { second_x, second_y }, 3.0f, color);
  }

}
