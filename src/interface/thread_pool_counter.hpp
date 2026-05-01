#ifndef _CGRAPH_INTERFACE_THREAD_POOL_COUNTER_HPP_
#define _CGRAPH_INTERFACE_THREAD_POOL_COUNTER_HPP_

#include "thread_pool.hpp"
#include <cstdint>

namespace CrocobyGraph {

  class ThreadPoolCounter {
    ThreadPool* thread_pool { nullptr };
    const uint8_t threads { 0 };

    void update_threads();

  public:
    void add_thread_pool(ThreadPool* thread_pool);
    void add_thread_pool(ThreadPool& thread_pool);

    explicit ThreadPoolCounter(uint8_t threads);
    ThreadPoolCounter(const ThreadPoolCounter& another) = delete;
    ThreadPoolCounter& operator=(const ThreadPoolCounter& another) = delete;
    ~ThreadPoolCounter();
};

}

#endif
