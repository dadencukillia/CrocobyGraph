#ifndef _CGRAPH_WINDOW_HPP_
#define _CGRAPH_WINDOW_HPP_

#include "../interface/ecs.hpp"
#include "../interface/scene.hpp"
#include "../interface/window_system.hpp"
#include "painter.hpp"
#include "raylib.h"

namespace CrocobyGraph {

  Camera2D get_camera_2d(const WindowInfo& info);

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
