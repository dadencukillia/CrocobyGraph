#ifndef _CGRAPH_RESOURCE_COUNTER_HPP_
#define _CGRAPH_RESOURCE_COUNTER_HPP_

#include <cassert>
#include <cstdint>

namespace CrocobyGraph {

  template <typename T>
  class ResourceCounter {
    static uint32_t& get_use_counter() {
      static uint32_t counter { 0 };

      return counter;
    }

    static const T*& get_resource() {
      static const T* resource { nullptr };

      return resource;
    }

    static bool is_resource_loaded() {
      return get_resource();
    }

    static void load_resource() {
      assert(!get_resource() && "The resource is already loaded");
      get_resource() = new T();
    }

    static void unload_resource() {
      assert(get_resource() && "The resource isn't loaded");
      delete get_resource();
      get_resource() = nullptr;
    }

  public:
    ResourceCounter() {
      inc();
    }

    ResourceCounter(const ResourceCounter<T>& another) = delete;
    ResourceCounter& operator=(const ResourceCounter& another) = delete;

    ~ResourceCounter() {
      dec();
    }

    const T& get() {
      if (!is_resource_loaded()) load_resource();

      return *get_resource();
    }

    static void inc() {
      get_use_counter()++;
    }

    static void dec() {
      auto val = --get_use_counter();
      if (val == 0 && is_resource_loaded()) {
        unload_resource();
      }
    }
  };

}

#endif
