#ifndef _CGRAPH_WINDOW_SYSTEM_HPP_
#define _CGRAPH_WINDOW_SYSTEM_HPP_

#include "ecs.hpp"
#include "systems.hpp"
#include <memory>
#include <string_view>
#include <vector>

namespace CrocobyGraph {

  class Window;

  struct CameraEntity {
    float zoom { 1.0f };
  };

  struct CursorEntity {
    bool left_button;
    bool middle_button;
    bool right_button;
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
    float mouse_local_x { 0.0f };
    float mouse_local_y { 0.0f };
    double delta { 0.0 };
  };

  class WindowUIFrame {
  public:
    virtual void load(GraphECS& ecs) {}
    virtual void draw(const WindowInfo& info, GraphECS& ecs) = 0;
    virtual ~WindowUIFrame() = default;
  };

  class WindowSystem : public ISystem {
    Window* window { nullptr };
    std::vector<std::unique_ptr<WindowUIFrame>> ui_frames;

  public:
    static const std::string_view system_name;

    WindowSystem() = default;
    WindowSystem(const WindowSystem&) = delete;
    WindowSystem& operator=(const WindowSystem&) = delete;
    ~WindowSystem();

    std::string_view get_system_name() override;
    void init_system(InitEvent ev) override;
    void on_tick(TickEvent ev) override;
    void on_remove(RemoveEvent ev) override;
    void add_ui_frame(std::unique_ptr<WindowUIFrame> frame);
  };

  std::unique_ptr<ISystem> get_window_system(std::vector<std::unique_ptr<WindowUIFrame>>&& frames = {});

  template<typename... Frames>
  std::unique_ptr<ISystem> get_window_system() {
    auto system = std::make_unique<WindowSystem>();

    (system->add_ui_frame(std::make_unique<Frames>()), ...);

    return system;
  }

}

#endif
