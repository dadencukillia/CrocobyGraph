#ifndef _CGRAPH_ENTITIES_HPP_
#define _CGRAPH_ENTITIES_HPP_

#include "color.hpp"
#include "../config.hpp"
#include "entt/entt.hpp"
#include <string>

namespace CrocobyGraph {

  enum class EdgeCurveType {
    Linear,
    Step,
    Ease
  };

  struct NodeEntity {
    Color color { DEFAULT_NODE_COLOR };
    float radius { DEFAULT_NODE_RADIUS };
  };

  struct EdgeEntity {
    entt::entity node_start { entt::null };
    entt::entity node_end { entt::null };
    bool arrow_on_start { false };
    bool arrow_on_end { false };
    Color color { DEFAULT_EDGE_COLOR };
    EdgeCurveType curve_type { EdgeCurveType::Linear };
  };

  struct LabelEntity {
    std::string label { "" };
    Color color { DEFAULT_LABEL_COLOR };
  };

}

#endif
