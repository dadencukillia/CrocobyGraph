#ifndef _CGRAPH_ENTITIES_HPP_
#define _CGRAPH_ENTITIES_HPP_

#include "color.hpp"
#include "entt/entt.hpp"
#include <string>

namespace CrocobyGraph {

  enum class EdgeCurveType {
    Linear,
    Step,
    Ease
  };

  struct NodeEntity {
    Color color { ColorPresets::WHITE };
    double radius { 20.0 };
  };

  struct EdgeEntity {
    entt::entity node_start { entt::null };
    entt::entity node_end { entt::null };
    bool arrow_on_start { false };
    bool arrow_on_end { false };
    Color color { ColorPresets::WHITE };
    EdgeCurveType curve_type { EdgeCurveType::Linear };
  };

  struct LabelEntity {
    std::string label { "" };
    Color color { ColorPresets::BLACK };
  };

}

#endif
