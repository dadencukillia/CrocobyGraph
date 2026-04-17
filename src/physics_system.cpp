#include "physics_system.hpp"
#include "physics.hpp"
#include <cassert>

namespace CrocobyGraph {

  PhysicsSystem::~PhysicsSystem() {
    assert(physics == nullptr && "For some reason on_remove haven't worked");
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

    return std::move(system);
  }

}
