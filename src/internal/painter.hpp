#ifndef _CGRAPH_PAINTER_HPP_
#define _CGRAPH_PAINTER_HPP_

#include "../interface/color.hpp"
#include "../interface/components.hpp"
#include "../interface/entities.hpp"
#include "../config.hpp"
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

    static void draw_node(Vector2 pos, Color color, float radius);
    static void draw_jelly_node(const std::vector<PositionComponent>& points, Vector2 center_pos, Color color);
    static void draw_edge(Vector2 from, Vector2 to, Color color, EdgeCurveType curve, float thickness = EDGES_THICKNESS);
    static void draw_self_loop(Vector2 pos, Color color, float radius, float thickness = EDGES_THICKNESS);
    void draw_label(Vector2 pos, std::string_view text, Color color) const;
    static void draw_arrow(Vector2 from, Vector2 to, float radius, Color color, EdgeCurveType curve, float thickness = EDGES_THICKNESS);
  };

}

#endif
