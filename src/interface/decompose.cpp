#include "decompose.hpp"
#include <cmath>
#include <cstddef>
#include <map>
#include <unordered_map>
#include <vector>

namespace CrocobyGraph {

  Batch decompose(std::vector<LayoutGraphNode>&& nodes) {
    Batch batch {};
    std::unordered_map<std::string, BeingCreatedEntity> name_ids;
    std::vector<BeingCreatedEntity> ids(nodes.size());

    uint32_t nodes_in_row = std::ceil(std::sqrt(static_cast<double>(nodes.size())));
    uint32_t rows = std::ceil(static_cast<double>(nodes.size()) / nodes_in_row);

    float offset_x = -(75.0f * (nodes_in_row - 1.0f)) / 2.0f;
    float offset_y = -(75.0f * (rows - 1.0f)) / 2.0f;
    for (size_t i = 0; i < nodes.size(); ++i) {
      auto& node = nodes[i];

      auto id = batch.add_node({
        .color = node.color,
        .radius = 20.0,
        .position = { 75.0f * (i % nodes_in_row) + offset_x, 75.0f * (i / nodes_in_row) + offset_y }
      });

      if (!node.label.empty()) {
        batch.add_label({
          .label = std::move(node.label),
        }, id);
      }

      if (!node.id.empty()) {
        name_ids.insert({ std::move(node.id), id });
      }

      ids[i] = id;
    }

    std::map<std::pair<size_t, size_t>, EdgeBundle> edges;

    for (size_t i = 0; i < nodes.size(); ++i) {
      auto& node = nodes[i];

      if (!node.points_to.empty()) {
        auto& a = ids[i];

        for (auto& point : node.points_to) {
          auto& b = name_ids[point];

          if (edges.contains({ b.id, a.id })) {
            edges.at({ b.id, a.id }).arrow_on_start = true;
          } else {
            edges.insert({ { a.id, b.id }, EdgeBundle{ 
              .node_start = a, 
              .node_end = b, 
              .arrow_on_start = false, 
              .arrow_on_end = true,
            } });
          }
        }
      }
    }

    for (auto [_k, v] : edges) {
      batch.add_edge(static_cast<EdgeBundle>(v));
    }

    return batch;
  }

}
