#ifndef _CGRAPH_PHYSICS_SYSTEM_HPP_
#define _CGRAPH_PHYSICS_SYSTEM_HPP_

#include "components.hpp"
#include "systems.hpp"
#include <memory>
#include <string_view>
#include <vector>

namespace CrocobyGraph {

  class Physics;

  struct VelocityComponent {
    float x { 0.0 };
    float y { 0.0 };
  };

  struct JellyComponent {
    std::vector<PositionComponent> points;
  };

  struct RepulsionComponent {
    float charge { 1.0 };
  };

  class PhysicsSystem : public ISystem {
    Physics* physics { nullptr };
    bool jelly { true };

  public:
    static const std::string_view system_name;

    PhysicsSystem() = default;
    PhysicsSystem(const PhysicsSystem&) = delete;
    PhysicsSystem& operator=(const PhysicsSystem&) = delete;
    ~PhysicsSystem();

    std::string_view get_system_name() override;
    void init_system(InitEvent ev) override;
    void on_tick(TickEvent ev) override;
    void on_remove(RemoveEvent ev) override;

    void no_jelly();
  };

  std::unique_ptr<ISystem> get_physics_system(bool jelly_effect = true);

}

#endif
