#include "ecs.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <string_view>
#include <thread>

namespace CrocobyGraph {

  GraphECS::~GraphECS() {
    clear_systems();
    delete scene;
  }

  void GraphECS::add_system(std::unique_ptr<ISystem> system) {
    new_systems_queue.push(std::move(system));
  }

  bool GraphECS::check_system(std::string_view system_name) {
      for (auto& system : systems) {
        if (system->get_system_name() == system_name) return true;
      }

      return false;
  }

  void clear_systems();

  void GraphECS::clear_systems() {
    if (update_busy) {
      for (size_t i = 0; i < systems.size(); ++i) {
        remove_list.push_back(i);
      }
    } else {
      for (auto& system : systems) {
        system->on_remove({ this });
      }

      remove_list.clear();
      systems.clear();
    }
  }

  void GraphECS::update(double dt) {
    assert(!update_busy && "Update can't be summoned in callbacks");
    update_busy = true;

    for (size_t i = 0; i < systems.size(); ++i) {
      auto& system = systems[i];
      bool should_remove = false;

      system->on_tick(TickEvent {
        .ecs = this,
        .delta_seconds = dt,
        .remove_system = [&]() { should_remove = true; }
      });

      if (should_remove) {
        remove_list.push_back(i);
      }
    }

    if (!remove_list.empty()) {
      std::sort(remove_list.begin(), remove_list.end());
      size_t last_removed_index = remove_list.back() + 1; // anti duplication

      for (auto it = remove_list.rbegin(); it != remove_list.rend(); ++it) {
        size_t idx = *it;

        if (last_removed_index <= idx || idx >= systems.size()) continue;
        last_removed_index = idx;

        systems[idx]->on_remove({ this });
        systems.erase(systems.begin() + idx);
      }

      remove_list.clear();
    }

    update_busy = false;
  }


  void GraphECS::run_loop() {
    assert(!loop_busy && "Already runned loop");
    loop_busy = true;

    using clock = std::chrono::high_resolution_clock;
    auto last_time = clock::now();
    const double target_dt = 1.0 / 60.0;

    add_systems_from_queue();

    while (!systems.empty()) {
      auto frame_start = clock::now();
      std::chrono::duration<double> elapsed = frame_start - last_time;
      double dt = elapsed.count();
      last_time = frame_start;

      update(dt);
      add_systems_from_queue();

      auto frame_end = clock::now();
      std::chrono::duration<double> work_time = frame_end - frame_start;
      if (work_time.count() < target_dt) {
        std::this_thread::sleep_for(std::chrono::duration<double>(target_dt - work_time.count()));
      }
    }

    loop_busy = false;
  }

  void GraphECS::add_systems_from_queue() {
    for (; !new_systems_queue.empty(); new_systems_queue.pop()) {
      auto new_system = std::move(new_systems_queue.front());

      new_system->init_system({ this });
      systems.push_back(std::move(new_system));
    }
  }

  Scene& GraphECS::get_scene() { return *scene; }

}
