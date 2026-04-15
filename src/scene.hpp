#ifndef _CGRAPH_SCENE_HPP_
#define _CGRAPH_SCENE_HPP_

#include "batch.hpp"
#include "entt/entt.hpp"

namespace CrocobyGraph {

  class Scene {
    entt::registry registry;

  public:
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;

    void remove(entt::entity id);
    void append(Batch&& batch);
    void clear();
    entt::registry& get_registry();

    template <typename It>
    void remove(It first, It last) {
      return this->registry.destroy(first, last);
    }
  };

}

#endif 
