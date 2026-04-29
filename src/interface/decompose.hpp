#ifndef _CGRAPH_INTERFACE_DECOMPOSE_HPP_
#define _CGRAPH_INTERFACE_DECOMPOSE_HPP_

#include "batch.hpp"
#include "layout.hpp"
#include <vector>

namespace CrocobyGraph {

  [[nodiscard]] Batch decompose(std::vector<LayoutGraphNode>&& nodes);

}

#endif
