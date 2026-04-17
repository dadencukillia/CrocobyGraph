#ifndef _CGRAPH_SYSTEMS_HPP_
#define _CGRAPH_SYSTEMS_HPP_

#include <functional>

namespace CrocobyGraph {

  class GraphECS;

  struct InitEvent {
    GraphECS* ecs;
  };

  struct TickEvent {
    GraphECS* ecs;
    double delta_seconds;
    std::function<void()> remove_system;
  };

  struct RemoveEvent {
    GraphECS* ecs;
  };

  class ISystem {
  public:
    virtual void init_system(InitEvent ev) {}
    virtual void on_tick(TickEvent ev) {
      ev.remove_system();
    }
    virtual void on_remove(RemoveEvent ev) {}

    virtual ~ISystem() = default;
  };

}

#endif
