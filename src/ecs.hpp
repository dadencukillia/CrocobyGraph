#ifndef _CGRAPH_ECS_HPP_
#define _CGRAPH_ECS_HPP_

#include "scene.hpp"
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <vector>

namespace CrocobyGraph {

  template <typename T>
  struct InitEvent {
    T* ecs;
  };

  template <typename T>
  struct TickEvent {
    T* ecs;
    double delta_seconds;
    std::function<void ()> remove_system;
  };

  template <typename T>
  struct RemoveEvent {
    T* ecs;
  };

  template <typename T>
  struct System {
    std::function<void (InitEvent<T>)> init_callback { [](InitEvent<T>){} };
    std::function<void (TickEvent<T>)> tick_callback { [](TickEvent<T> e){ e.remove_system(); } };
    std::function<void (RemoveEvent<T>)> remove_callback { [](RemoveEvent<T>){} };
  };

  class GraphECS {
  public:
    using tick_callback = std::function<void (TickEvent<GraphECS>)>;
    using remove_callback = std::function<void (RemoveEvent<GraphECS>)>;

    GraphECS() = default;
    GraphECS(const GraphECS&) = delete;
    GraphECS& operator=(const GraphECS&) = delete;

    GraphECS(GraphECS&& other) noexcept;
    GraphECS& operator=(GraphECS&& other) noexcept;

    ~GraphECS();

    void add_system(System<GraphECS> system);
    void clear_systems();
    void update(double dt);
    void run_loop();

    Scene* get_scene();

  private:
    std::vector<tick_callback> tick_callbacks;
    std::vector<remove_callback> remove_callbacks;
    std::vector<size_t> remove_list;
    Scene* scene { new Scene() };
    bool update_busy { false };
    bool loop_busy { false };
  };

}

#endif
