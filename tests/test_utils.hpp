#pragma once

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace TestUtils {

inline int g_failures = 0;
inline int g_checks = 0;
inline const char* g_current_test = nullptr;

#define TEST_CASE(name) \
  static void test_##name(); \
  static struct Register_##name { \
    Register_##name() { \
      ::TestUtils::g_current_test = #name; \
      std::cout << "[TEST] " << #name << "\n"; \
      test_##name(); \
    } \
  } register_##name##_instance; \
  static void test_##name()

#define CHECK(expr) \
  do { \
    ++::TestUtils::g_checks; \
    if (!(expr)) { \
      ++::TestUtils::g_failures; \
      std::cerr << "  FAIL: " << #expr << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
    } \
  } while(0)

#define CHECK_EQ(a, b) \
  do { \
    ++::TestUtils::g_checks; \
    auto _a = (a); \
    auto _b = (b); \
    if (!(_a == _b)) { \
      ++::TestUtils::g_failures; \
      std::cerr << "  FAIL: " << #a << " == " << #b \
                << "  [got: " << _a << " != " << _b << "]" \
                << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
    } \
  } while(0)

#define CHECK_THROWS(expr) \
  do { \
    ++::TestUtils::g_checks; \
    bool _threw = false; \
    try { (expr); } catch (...) { _threw = true; } \
    if (!_threw) { \
      ++::TestUtils::g_failures; \
      std::cerr << "  FAIL: expected exception from: " << #expr \
                << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
    } \
  } while(0)

#define CHECK_NOTHROW(expr) \
  do { \
    ++::TestUtils::g_checks; \
    try { (expr); } \
    catch (...) { \
      ++::TestUtils::g_failures; \
      std::cerr << "  FAIL: unexpected exception from: " << #expr \
                << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
    } \
  } while(0)

inline int report_and_exit() {
  std::cout << "\n=== Results: " << (g_checks - g_failures) << "/" << g_checks
            << " passed";
  if (g_failures > 0) {
    std::cout << ", " << g_failures << " FAILED";
  }
  std::cout << " ===\n";
  return g_failures > 0 ? 1 : 0;
}

} // namespace TestUtils