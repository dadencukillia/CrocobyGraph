#include "window_system.hpp"
#include "ecs.hpp"
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
    window->init(gui, &ev.ecs->get_scene(), ev.ecs);
  }

  void WindowSystem::on_tick(TickEvent ev) {
    window->draw();
    window->update(ev.delta_seconds);
  }

  void WindowSystem::on_remove(RemoveEvent ev) {
    delete window;
    window = nullptr;
  }

  void WindowSystem::no_gui() {
    gui = false;
  }

  std::unique_ptr<ISystem> get_window_system(bool gui) {
    auto system = std::make_unique<WindowSystem>();
    if (!gui) system->no_gui();

    return std::move(system);
  }

}
