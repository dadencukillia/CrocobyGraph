#include "physics_system.hpp"
#include "../internal/physics.hpp"
#include <cassert>
#include <string_view>

namespace CrocobyGraph {

  const std::string_view PhysicsSystem::system_name { "dev.crocobygraph.physics" };

  PhysicsSystem::~PhysicsSystem() {
    assert(physics == nullptr && "For some reason on_remove haven't worked");
  }

  std::string_view PhysicsSystem::get_system_name() {
    return system_name;
  }

  void PhysicsSystem::init_system(InitEvent ev) {
    physics = new Physics();
    physics->init(jelly, &ev.ecs->get_scene(), ev.ecs);
  }

  void PhysicsSystem::on_tick(TickEvent ev) {
    physics->update(ev.delta_seconds);
  }

  void PhysicsSystem::on_remove(RemoveEvent ev) {
    delete physics;
    physics = nullptr;
  }

  void PhysicsSystem::no_jelly() {
    jelly = false;
  }

  std::unique_ptr<ISystem> get_physics_system(bool jelly_effect) {
    auto system = std::make_unique<PhysicsSystem>();
    if (!jelly_effect) system->no_jelly();

    return system;
  }

}
