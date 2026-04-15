#include "window_system.hpp"
#include "ecs.hpp"
#include "window.hpp"

namespace CrocobyGraph {

  System<GraphECS> get_window_system(bool info_gui) {
    auto window = new Window();

    return {
      .init_callback = [window, info_gui](InitEvent<GraphECS> ev) {
        window->init(info_gui, ev.ecs->get_scene(), ev.ecs);
      },
      .tick_callback = [window](TickEvent<GraphECS> ev) {
        window->draw();
        window->update(ev.delta_seconds);
      },
      .remove_callback = [window](auto ev) {
        delete window;
      }
    };
  }

}
