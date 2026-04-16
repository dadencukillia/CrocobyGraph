#ifndef _CGRAPH_PHYSICS_HPP_
#define _CGRAPH_PHYSICS_HPP_

#include "components.hpp"
#include "ecs.hpp"
#include "scene.hpp"
#include <cstddef>
#include <vector>

namespace CrocobyGraph {

  inline const size_t jelly_points = 64;

  struct VelocityComponent {
    float x;
    float y;
  };

  struct JellyComponent {
    std::vector<PositionComponent> points;
  };

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
