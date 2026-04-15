#ifndef _CGRAPH_LAYOUT_HPP_
#define _CGRAPH_LAYOUT_HPP_

#include "color.hpp"
#include <cstddef>
#include <vector>

namespace CrocobyGraph {

  struct LayoutGraphNode {
    std::string label { "" };
    Color color { Colors::WHITE };
    std::vector<std::string> points_to;
    std::string id { "" };
  };

  namespace aliases {
    using LGN = LayoutGraphNode;
  }

  template <unsigned int N>
  inline std::vector<LayoutGraphNode> layout_from_adjacency_matrix(
    const std::string (&nodes)[N],
    const bool (&connections)[N][N],
    const Color& colored = Colors::WHITE
  ) {
    std::vector<LayoutGraphNode> layout;
    layout.reserve(N);

    for (size_t i = 0; i < N; ++i) {
      LayoutGraphNode& node = layout.emplace_back();

      node.id = "node" + std::to_string(i + 1);
      node.label = nodes[i];
      node.color = colored;
    }

    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        if (connections[i][j]) {
          layout[i].points_to.push_back(layout[j].id);
        }
      }
    }

    return layout;
  }

}

#endif
