#ifndef _CGRAPH_PAINTER_HPP_
#define _CGRAPH_PAINTER_HPP_

#include "color.hpp"
#include "components.hpp"
#include "entities.hpp"
#include <string_view>
#include <vector>
#include "raylib.h"

namespace CrocobyGraph {

  class Painter {
    Font open_sans_font;

  public:
    Painter() = default;
    ~Painter();

    void load();

    void draw_node(Vector2 pos, Color color, double radius) const;
    void draw_jelly_node(const std::vector<PositionComponent>& points, Vector2 center_pos, Color color) const;
    void draw_edge(Vector2 from, Vector2 to, Color color, EdgeCurveType curve) const;
    void draw_self_loop(Vector2 pos, Color color, double radius) const;
    void draw_label(Vector2 pos, std::string_view text, Color color) const;
    void draw_arrow(Vector2 from, Vector2 to, float radius, Color color, EdgeCurveType curve) const;
  };

}

#endif
