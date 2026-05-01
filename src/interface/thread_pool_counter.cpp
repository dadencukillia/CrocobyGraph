#include "thread_pool_counter.hpp"
#include <algorithm>
#include <cassert>

namespace CrocobyGraph {

  void ThreadPoolCounter::update_threads() {
    uint8_t threads { 0 };
    if (!thread_pool->threads_stack.empty()) {
      threads = std::ranges::max(thread_pool->threads_stack);
    }

    thread_pool->set_threads_count(threads);
  }

  void ThreadPoolCounter::add_thread_pool(ThreadPool* thread_pool) {
    assert(this->thread_pool == nullptr && "Field is already set");
    this->thread_pool = thread_pool;
    thread_pool->threads_stack.push_back(threads);
    update_threads();
  }

  void ThreadPoolCounter::add_thread_pool(ThreadPool& thread_pool) {
    add_thread_pool(&thread_pool);
  }

  ThreadPoolCounter::ThreadPoolCounter(uint8_t threads) :
    threads { threads } {}

  ThreadPoolCounter::~ThreadPoolCounter() {
    auto it = std::ranges::find(thread_pool->threads_stack, threads);
    if (it != thread_pool->threads_stack.end()) {
      thread_pool->threads_stack.erase(it);
    }

    update_threads();
  }

}
