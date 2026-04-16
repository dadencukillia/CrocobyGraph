#include "physics_system.hpp"
#include "physics.hpp"

namespace CrocobyGraph {

  System get_physics_system(bool jelly_effect) {
    auto physics = new Physics();

    return {
      .init_callback = [physics, jelly_effect](auto ev) {
        physics->init(jelly_effect, &ev.ecs->get_scene(), ev.ecs);
      },
      .tick_callback = [physics](auto ev) {
        physics->update(ev.delta_seconds);
      },
      .remove_callback = [physics](auto ev) {
        delete physics;
      }
    };
  }
}
