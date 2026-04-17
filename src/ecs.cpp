#include "ecs.hpp"
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <numeric>
#include <thread>
#include <utility>

namespace CrocobyGraph {

  GraphECS::~GraphECS() {
    clear_systems();
    delete scene;
  }

  void GraphECS::add_system(System&& system) {
    new_systems_queue.push(std::move(system));
  }

  void GraphECS::clear_systems() {
    if (update_busy) {
      remove_list.resize(remove_callbacks.size());
      std::iota(remove_list.begin(), remove_list.end(), 0);
    } else {
      for (auto& remove_cb : remove_callbacks) {
          if (remove_cb) remove_cb({ this });
      }

      tick_callbacks.clear();
      remove_callbacks.clear();
    }
  }

  void GraphECS::update(double dt) {
    assert(!update_busy && "Update can't be summoned in callbacks");
    update_busy = true;

    for (size_t i = 0; i < tick_callbacks.size(); ++i) {
      bool should_remove = false;

      TickEvent event {
        .ecs = this,
        .delta_seconds = dt,
        .remove_system = [&]() { should_remove = true; }
      };

      tick_callbacks[i](event);

      if (should_remove) {
        remove_list.push_back(i);
      }
    }

    for (auto it = remove_list.rbegin(); it != remove_list.rend(); ++it) {
      size_t idx = *it;

      if (remove_callbacks[idx]) {
          remove_callbacks[idx]({ this });
      }

      tick_callbacks.erase(tick_callbacks.begin() + idx);
      remove_callbacks.erase(remove_callbacks.begin() + idx);
    }

    remove_list.clear();

    update_busy = false;
  }

  void GraphECS::run_loop() {
    assert(!loop_busy && "Already runned loop");
    loop_busy = true;

    using clock = std::chrono::high_resolution_clock;
    auto last_time = clock::now();
    const double target_dt = 1.0 / 60.0;

    add_systems_from_queue();

    while (!tick_callbacks.empty()) {
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
      auto& new_system = new_systems_queue.front();

      new_system.init_callback({ this });
      tick_callbacks.push_back(std::move(new_system.tick_callback));
      remove_callbacks.push_back(std::move(new_system.remove_callback));
    }
  }

  Scene& GraphECS::get_scene() const { return *scene; }

}
