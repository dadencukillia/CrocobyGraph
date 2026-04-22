#ifndef _CGRAPH_SCENE_HPP_
#define _CGRAPH_SCENE_HPP_

#include "batch.hpp"
#include "entt/entity/fwd.hpp"

namespace CrocobyGraph {

  class Scene {
    entt::registry* registry;

  public:
    Scene();
    ~Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;

    void append(Batch&& batch, float offset_x = 0.0f, float offset_y = 0.0f);
    void clear();
    entt::registry& get_registry();
  };

}

#endif 
