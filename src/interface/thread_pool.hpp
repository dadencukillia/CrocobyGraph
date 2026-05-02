#ifndef _CGRAPH_INTERFACE_THREAD_POOL_HPP_
#define _CGRAPH_INTERFACE_THREAD_POOL_HPP_

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace CrocobyGraph {

  class ThreadPool {
    struct {
      std::thread::id main_thread { std::this_thread::get_id() };
      std::vector<uint8_t> threads_stack;
      std::vector<std::thread> threads;
      size_t tasks_counter { 0 };
      uint8_t threads_count { 0 };
    } only_main_access;

    alignas(64) struct {
      bool should_stop { false };

      std::queue<std::pair<size_t, std::function<void()>>> tasks;
      std::mutex tasks_mutex;
      std::condition_variable task_condition;

      std::vector<uint8_t> tasks_status;
      std::mutex tasks_status_mutex;
      std::condition_variable complete_condition;
      size_t tasks_active { 0 };
    } both_access;

    std::thread spawn_thread();
    void flush();
    void set_threads_count(uint8_t new_count);

    friend class GraphECS;
    friend class ThreadPoolCounter;

  public:
    ThreadPool() = default;
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool();

    size_t enqueue_task(std::function<void()>&& task);
    [[nodiscard]] bool task_completed(size_t task);
    void wait_for_task(size_t task);
  };

}

#endif
