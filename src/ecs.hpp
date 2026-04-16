#ifndef _CGRAPH_ECS_HPP_
#define _CGRAPH_ECS_HPP_

#include "scene.hpp"
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <vector>

namespace CrocobyGraph {

  class GraphECS;

  struct InitEvent {
    GraphECS* ecs;
  };

  struct TickEvent {
    GraphECS* ecs;
    double delta_seconds;
    std::function<void ()> remove_system;
  };

  struct RemoveEvent {
    GraphECS* ecs;
  };

  struct System {
    std::function<void (InitEvent)> init_callback { [](InitEvent){} };
    std::function<void (TickEvent)> tick_callback { [](TickEvent e){ e.remove_system(); } };
    std::function<void (RemoveEvent)> remove_callback { [](RemoveEvent){} };
  };

  class GraphECS {
  public:
    using tick_callback = std::function<void (TickEvent)>;
    using remove_callback = std::function<void (RemoveEvent)>;

    GraphECS() = default;
    GraphECS(const GraphECS&) = delete;
    GraphECS& operator=(const GraphECS&) = delete;

    GraphECS(GraphECS&& other) noexcept;
    GraphECS& operator=(GraphECS&& other) noexcept;

    ~GraphECS();

    void add_system(System system);
    void clear_systems();
    void update(double dt);
    void run_loop();

    Scene& get_scene();

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
