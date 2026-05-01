#include "thread_pool.hpp"
#include <cstdint>
#include <iostream>

namespace CrocobyGraph {

  std::thread ThreadPool::spawn_thread() {
    const uint8_t thread_id = threads_count;
    std::cout << "Thread #" << static_cast<int>(thread_id) << " opened\n";

    return std::thread([this, thread_id]() {
      while (true) {
        std::unique_lock<std::mutex> locker(tasks_mutex);

        condition.wait(locker, [this, thread_id]() {
          return !tasks.empty() || should_stop || threads_count <= thread_id;
        });

        if (should_stop || threads_count <= thread_id) {
          std::cout << "Thread #" << static_cast<int>(thread_id) << " closed\n";
          return;
        }

        auto task { std::move(tasks.front()) };
        tasks.pop();

        locker.unlock();

        try {
          task.second();
        } catch (...) {}

        {
          std::lock_guard<std::mutex> locker(tasks_status_mutex);
          tasks_status[task.first] = true;
          tasks_active--;
        }
        complete_condition.notify_all();
      }
    });
  }

  void ThreadPool::flush() {
    if (std::this_thread::get_id() != main_thread) throw std::runtime_error("Not accessible for calling not from the main thread");
    std::unique_lock<std::mutex> locker(tasks_status_mutex);
    complete_condition.wait(locker, [this]() {
      return tasks_active == 0;
    });

    tasks_counter = 0;
    tasks_status.clear();
  }

  void ThreadPool::set_threads_count(uint8_t new_count) {
    if (std::this_thread::get_id() != main_thread) throw std::runtime_error("Not accessible for calling not from the main thread");
    if (new_count > threads_count) {
      for (uint8_t i = threads_count; i < new_count; ++i) {
        threads.push_back(spawn_thread());
        ++threads_count;
      }
    } else if (new_count < threads_count) {
      std::vector<std::thread> to_remove;
      to_remove.reserve(threads_count - new_count);

      for (uint8_t i = threads_count; i > new_count; --i) {
        --threads_count;
        to_remove.push_back(std::move(threads.back()));
        threads.pop_back();
      }

      condition.notify_all();
      for (auto& thread : to_remove) {
        if (thread.joinable()) thread.join();
      }
    }
  }

  ThreadPool::~ThreadPool() {
    try {
      flush();
    } catch (...) {}

    should_stop = true;
    set_threads_count(0);
  }

  size_t ThreadPool::enqueue_task(std::function<void()>&& task) {
    if (std::this_thread::get_id() != main_thread) throw std::runtime_error("Not accessible for calling not from the main thread");
    size_t task_id { tasks_counter++ };

    {
      std::scoped_lock locker(tasks_mutex, tasks_status_mutex);
      tasks.push({ task_id, std::move(task) });
      tasks_active++;
      tasks_status.push_back(false);
    }
    condition.notify_one();

    return task_id;
  }

  bool ThreadPool::task_completed(size_t task) {
    std::lock_guard<std::mutex> locker(tasks_status_mutex);

    return tasks_status[task];
  }

  void ThreadPool::wait_for_task(size_t task) {
    if (std::this_thread::get_id() != main_thread) throw std::runtime_error("Not accessible for calling not from the main thread");
    std::unique_lock<std::mutex> locker(tasks_status_mutex);
    complete_condition.wait(locker, [this, task]() {
      return tasks_status[task];
    });
  }

}
