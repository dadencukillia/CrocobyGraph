#ifndef _CGRAPH_SCENE_HPP_
#define _CGRAPH_SCENE_HPP_

#include "batch.hpp"
#include "entities.hpp"
#include <vector>
#include "entt/entt.hpp"

namespace CrocobyGraph {

  class Scene {
    entt::registry registry;

  public:
    Scene() = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;

    ~Scene();

    void remove(entt::entity id);
    void append(Batch&& batch);
    void clear();

    template <typename It>
    void remove(It first, It last) {
      return this->registry.destroy(first, last);
    }

    template <typename... Type, typename... Exclude>
    auto view(entt::exclude_t<Exclude...> exclude = {}) const {
      return this->registry.view<Type...>(exclude);
    }
  };

}

#endif 
