#ifndef _CGRAPH_PAINTER_HPP_
#define _CGRAPH_PAINTER_HPP_

#include "color.hpp"
#include "entities.hpp"
#include <string_view>

namespace CrocobyGraph {

  class Painter {
  public:
    Painter() = default;

    void draw_node(float x, float y, Color color, double radius);
    void draw_edge(float from_x, float from_y, float to_x, float to_y, Color color, EdgeCurveType curve);
    void draw_label(float x, float y, std::string_view text, Color color);
    void draw_arrow(float from_x, float from_y, float to_x, float to_y, float radius, Color color);
  };

}

#endif
