#ifndef _CGRAPH_ECS_HPP_
#define _CGRAPH_ECS_HPP_

#include "scene.hpp"
#include "systems.hpp"
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <queue>
#include <string_view>
#include <vector>

namespace CrocobyGraph {

  class GraphECS {
    std::queue<std::unique_ptr<ISystem>> new_systems_queue;
    std::vector<std::unique_ptr<ISystem>> systems;
    std::vector<size_t> remove_list;
    Scene* scene { new Scene() };
    bool update_busy { false };
    bool loop_busy { false };

    void add_systems_from_queue();

  public:
    GraphECS() = default;
    GraphECS(const GraphECS&) = delete;
    GraphECS& operator=(const GraphECS&) = delete;
    GraphECS(GraphECS&& other) noexcept = delete;
    GraphECS& operator=(GraphECS&& other) noexcept = delete;

    ~GraphECS();

    void add_system(std::unique_ptr<ISystem> system);
    size_t remove_systems(std::string_view system_name);
    bool check_system(std::string_view system_name);
    void clear_systems();
    void update(double dt);
    void run_loop();

    template <typename T, typename... Args>
    void add_system(Args&&... args) {
      auto new_sys = std::make_unique<T>(std::forward<Args>(args)...);
      add_system(new_sys);
    }

    Scene& get_scene();
  };

}

#endif
