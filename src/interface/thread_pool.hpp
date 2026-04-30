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
    std::thread::id main_thread { std::this_thread::get_id() };
    std::queue<std::pair<size_t, std::function<void()>>> tasks;
    bool should_stop { false };
    std::mutex tasks_mutex;
    std::condition_variable condition;

    size_t tasks_counter { 0 };
    std::vector<uint8_t> tasks_status;
    std::mutex tasks_status_mutex;
    std::condition_variable complete_condition;
    size_t tasks_active { 0 };

    std::vector<std::thread> threads;
    uint8_t threads_count { 0 };

    std::thread spawn_thread();
    void flush();

    friend class GraphECS;

  public:
    ThreadPool() = default;
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool();

    void set_threads_count_force(uint8_t new_count);
    void set_minimum_threads(uint8_t minimum);
    size_t enqueue_task(std::function<void()>&& task);
    [[nodiscard]] bool task_completed(size_t task);
    void wait_for_task(size_t task);
  };

}

#endif
