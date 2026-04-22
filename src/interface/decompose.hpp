#ifndef _CGRAPH_DECOMPOSE_HPP_
#define _CGRAPH_DECOMPOSE_HPP_

#include "batch.hpp"
#include "layout.hpp"
#include <vector>

namespace CrocobyGraph {

  Batch decompose(std::vector<LayoutGraphNode>&& nodes);

}

#endif
