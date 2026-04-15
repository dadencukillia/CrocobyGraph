#ifndef _CGRAPH_ENTITIES_HPP_
#define _CGRAPH_ENTITIES_HPP_

#include "color.hpp"
#include "entt/entt.hpp"
#include <string>

namespace CrocobyGraph {

  enum class EdgeCurveType {
    Linear,
    SlowMiddle,
    Ease
  };

  struct NodeEntity {
    Color color { Colors::WHITE };
    double radius { 1.0 };
  };

  struct EdgeEntity {
    entt::entity node_start { entt::null };
    entt::entity node_end { entt::null };
    bool arrow_on_start { false };
    bool arrow_on_end { false };
    Color color { Colors::WHITE };
    EdgeCurveType curve_type { EdgeCurveType::Linear };
  };

  struct LabelEntity {
    std::string label { "" };
    Color color { Colors::BLACK };
  };

}

#endif
