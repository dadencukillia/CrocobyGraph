#ifndef _CGRAPH_WINDOW_SYSTEM_HPP_
#define _CGRAPH_WINDOW_SYSTEM_HPP_

#include "ecs.hpp"

namespace CrocobyGraph {

  System<GraphECS> get_window_system(bool debug_gui = true);

}

#endif
