#include "painter.hpp"
#include "raylib.h"

namespace CrocobyGraph {

  void Painter::draw_node(float x, float y, Color color, double radius) {
    DrawCircle(30 * x, 30 * y, static_cast<float>(radius * 30), ::Color(color.get_red(), color.get_green(), color.get_blue(), color.get_alpha()));
  }

  void Painter::draw_edge(float from_x, float from_y, float to_x, float to_y, Color color, EdgeCurveType curve) {
    DrawLineEx({ 30 * from_x, 30 * from_y }, { 30 * to_x, 30 * to_y }, 3.0f, ::Color(color.get_red(), color.get_green(), color.get_blue(), color.get_alpha()));
  }

  void Painter::draw_label(float x, float y, std::string_view text, Color color) {
  }

  void Painter::draw_arrow(float x, float y, double angle, Color color) {
  }

}
