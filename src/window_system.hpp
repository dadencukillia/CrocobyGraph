#ifndef _CGRAPH_WINDOW_SYSTEM_HPP_
#define _CGRAPH_WINDOW_SYSTEM_HPP_

#include "systems.hpp"
#include <memory>

namespace CrocobyGraph {

  class Window;

  class WindowSystem : public ISystem {
  public:
    WindowSystem() = default;
    WindowSystem(const WindowSystem&) = delete;
    WindowSystem& operator=(const WindowSystem&) = delete;
    ~WindowSystem();

    void init_system(InitEvent ev) override;
    void on_tick(TickEvent ev) override;
    void on_remove(RemoveEvent ev) override;

    void no_gui();

  private:
    Window* window { nullptr };
    bool gui { true };
  };

  std::unique_ptr<ISystem> get_window_system(bool gui = true);

}

#endif
