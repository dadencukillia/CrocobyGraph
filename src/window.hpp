#ifndef _CGRAPH_WINDOW_HPP_
#define _CGRAPH_WINDOW_HPP_

#include "ecs.hpp"
#include "painter.hpp"
#include "scene.hpp"

namespace CrocobyGraph {

  struct CameraEntity {
    float zoom { 1.0f };
  };

  struct WindowInfo {
    float mouse_x { 0.0f };
    float mouse_y { 0.0f };
    int width { 0 };
    int height { 0 };
    bool left_button_pressed { false };
    bool right_button_pressed { false };
    bool middle_button_pressed { false };
    float camera_x { 0.0f };
    float camera_y { 0.0f };
    float camera_zoom { 1.0f };
    float camera_border_left { 0.0f };
    float camera_border_right { 0.0f };
    float camera_border_top { 0.0f };
    float camera_border_bottom { 0.0f };
    float cursor_local_position_x { 0.0f };
    float cursor_local_position_y { 0.0f };
  };

  struct GUIState {};

  class Window {
    Painter painter {};
    Scene* scene { nullptr };
    GraphECS* ecs { nullptr };
    WindowInfo window_states { };
    GUIState gui_states { };
    bool with_gui { false };

  public:
    Window() = default;
    ~Window();

    void init(bool gui, Scene* scene, GraphECS* ecs);

    void draw_background();
    void draw_components();
    void draw_gui();

    void draw();
    void update(double delta);
  };

};

#endif
