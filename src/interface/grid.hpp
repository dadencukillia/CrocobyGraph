#ifndef _CGRAPH_INTERFACE_GRID_HPP_
#define _CGRAPH_INTERFACE_GRID_HPP_

#include "batch.hpp"
#include <cstdint>

namespace CrocobyGraph {

  [[nodiscard]] Batch generate_grid(uint32_t width, uint32_t height, float spacing = 60.0f);

}

#endif
