#ifndef _CGRAPH_ENTITIES_HPP_
#define _CGRAPH_ENTITIES_HPP_

#include "color.hpp"
#include "position.hpp"
#include <string>

namespace CrocobyGraph {

  enum class EdgeCurveType {
    Linear,
    SlowMiddle,
    Ease
  };

  struct EntityGraphNode {
    Position position { };
    Color color { Colors::WHITE };
    double radius { 1.0 };
  };

  struct EntityGraphEdge {
    EntityGraphNode* start { nullptr };
    EntityGraphNode* end { nullptr };
    bool arrowOnStart { false };
    bool arrowOnEnd { false };
    Color color { Colors::WHITE };
    EdgeCurveType curveType { EdgeCurveType::Linear };
  };

  struct EntityGraphNodeLabel {
    std::string label { "" };
    Color color { Colors::BLACK };
    EntityGraphNode* node { nullptr };
  };

  struct EntityGraphEdgeLabel {
    std::string label { "" };
    Color color { Colors::BLACK };
    EntityGraphEdge* edge { nullptr };
  };

  struct EntityGraphFreeLabel {
    std::string label { "" };
    Color color { Colors::BLACK };
    Position position { };
  };

  namespace aliases {
    using EGN = EntityGraphNode;
    using EGE = EntityGraphEdge;
    using EGNL = EntityGraphNodeLabel;
    using EGEL = EntityGraphEdgeLabel;
    using EGFL = EntityGraphFreeLabel;
  }

}

#endif
