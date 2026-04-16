#ifndef _CGRAPH_PHYSICS_HPP_
#define _CGRAPH_PHYSICS_HPP_

#include "ecs.hpp"
#include "scene.hpp"

namespace CrocobyGraph {

  struct VelocityComponent {
    float x;
    float y;
  };

  class Physics {
    Scene* scene { nullptr };
    GraphECS* ecs { nullptr };
    bool with_jelly { false };

  public:
    Physics() = default;
    ~Physics();

    void init(bool jelly, Scene* scene, GraphECS* ecs);

    void update(double delta);
    void update_velocity(double delta);
    void move(double delta);
  };

}

#endif
