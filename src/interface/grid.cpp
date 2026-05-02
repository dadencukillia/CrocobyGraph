#include "grid.hpp"

namespace CrocobyGraph {

  [[nodiscard]] Batch generate_grid(uint32_t width, uint32_t height, float spacing) {
    Batch mb;

    const float offset_x { (1.0f - static_cast<float>(width)) * spacing * 0.5f };
    const float offset_y { (1.0f - static_cast<float>(height)) * spacing * 0.5f };

    std::vector<BeingCreatedEntity> prev_row(width);

    for (uint32_t y = 0; y < height; ++y) {
      std::vector<BeingCreatedEntity> cur_row(width);

      for (uint32_t x = 0; x < width; ++x) {
        cur_row[x] = mb.add_node({
          .position = { x * spacing + offset_x, y * spacing + offset_y }
        });

        if (x != 0) {
          mb.add_edge({
            .node_start = cur_row[x - 1],
            .node_end = cur_row[x],
          });
        }
        if (y != 0) {
          mb.add_edge({
            .node_start = prev_row[x],
            .node_end = cur_row[x]
          });
        }
      }

      prev_row = std::move(cur_row);
    }

    return mb;
  }

}
