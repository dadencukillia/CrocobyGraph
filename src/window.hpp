#ifndef _CGRAPH_WINDOW_HPP_
#define _CGRAPH_WINDOW_HPP_

#include "ecs.hpp"
#include "painter.hpp"
#include "scene.hpp"
#include "window_system.hpp"

namespace CrocobyGraph {

  class Window {
    Painter painter {};
    Scene* scene { nullptr };
    GraphECS* ecs { nullptr };
    WindowInfo window_states { };
    std::vector<std::unique_ptr<WindowUIFrame>> ui_frames;

  public:
    Window() = default;
    ~Window();

    void init(std::vector<std::unique_ptr<WindowUIFrame>>&& ui_frames, Scene* scene, GraphECS* ecs);

    void draw_background();
    void draw_components();
    void draw_gui();

    void draw();
    void update(double delta);
  };

};

#endif
