#include "window_system.hpp"
#include "ecs.hpp"
#include "raylib.h"
#include "window.hpp"
#include <cassert>
#include <memory>

namespace CrocobyGraph {

  const std::string_view WindowSystem::system_name { "dev.crocobygraph.window" };

  WindowSystem::~WindowSystem() {
    assert(window == nullptr && "For some reason on_remove haven't worked");
  }

  std::string_view WindowSystem::get_system_name() {
    return system_name;
  }

  void WindowSystem::init_system(InitEvent ev) {
    window = new Window();
    window->init(std::move(ui_frames), &ev.ecs->get_scene(), ev.ecs);
  }

  void WindowSystem::on_tick(TickEvent ev) {
    window->update(ev.delta_seconds);
    window->draw();

    if (WindowShouldClose()) {
      ev.ecs->clear_systems();
      return;
    }
  }

  void WindowSystem::on_remove(RemoveEvent ev) {
    delete window;
    window = nullptr;
  }

  void WindowSystem::add_ui_frame(std::unique_ptr<WindowUIFrame> frame) {
    ui_frames.push_back(std::move(frame));
  }

  std::unique_ptr<ISystem> get_window_system(std::vector<std::unique_ptr<WindowUIFrame>>&& frames) {
    auto system = std::make_unique<WindowSystem>();

    for (auto& frame : frames) {
      system->add_ui_frame(std::move(frame));
    }

    return system;
  }

}
