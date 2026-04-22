#ifndef _CGRAPH_PHYSICS_HPP_
#define _CGRAPH_PHYSICS_HPP_

#include "../interface/components.hpp"
#include "../interface/ecs.hpp"
#include "../interface/scene.hpp"
#include <cstddef>

namespace CrocobyGraph {

  inline const size_t jelly_points = 32;

  class Physics {
    Scene* scene { nullptr };
    GraphECS* ecs { nullptr };
    bool with_jelly { false };
    PositionComponent jelly_ideal_points[jelly_points] {};

  public:
    Physics() = default;
    ~Physics();

    void init(bool jelly, Scene* scene, GraphECS* ecs);

    void update(double delta);
    void update_velocity(double delta);
    void move(double delta);
    void update_jelly(double delta);
  };

}

#endif
