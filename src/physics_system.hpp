#ifndef _CGRAPH_PHYSICS_SYSTEM_HPP_
#define _CGRAPH_PHYSICS_SYSTEM_HPP_

#include "systems.hpp"
#include <memory>
#include <string_view>

namespace CrocobyGraph {

  class Physics;

  class PhysicsSystem : public ISystem {
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

  private:
    Physics* physics { nullptr };
    bool jelly { true };
  };

  std::unique_ptr<ISystem> get_physics_system(bool jelly_effect = true);

}

#endif
