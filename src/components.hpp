#ifndef _CGRAPH_COMPONENTS_HPP_
#define _CGRAPH_COMPONENTS_HPP_

#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"

namespace CrocobyGraph {

  struct PositionComponent {
    double x { 0.0 };
    double y { 0.0 };
  };

  struct AttachComponent {
    entt::entity target { entt::null };
    double offset_x { 0.0 };
    double offset_y { 0.0 };
  };

}

#endif
