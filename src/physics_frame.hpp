#ifndef _CGRAPH_PHYSICS_FRAME_HPP_
#define _CGRAPH_PHYSICS_FRAME_HPP_

#include "ecs.hpp"
#include "window_system.hpp"

namespace CrocobyGraph {

  class PhysicsFrame : public WindowUIFrame {
    bool jelly_toggle { true };
    float mouse_repulsion { 0.0f };

    void apply_mouse_repulsion(const WindowInfo& info, GraphECS& ecs);

  public:
    PhysicsFrame() = default;
    PhysicsFrame(const PhysicsFrame&) = delete;
    PhysicsFrame& operator=(const PhysicsFrame&) = delete;
    ~PhysicsFrame();

    void draw(const WindowInfo& info, GraphECS& ecs) override;
  };

}

#endif
