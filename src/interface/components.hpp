#ifndef _CGRAPH_COMPONENTS_HPP_
#define _CGRAPH_COMPONENTS_HPP_

#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"

namespace CrocobyGraph {

  struct PositionComponent {
    float x { 0.0f };
    float y { 0.0f };
  };

  struct AttachComponent {
    entt::entity target { entt::null };
    float offset_x { 0.0f };
    float offset_y { 0.0f };
  };

}

#endif
