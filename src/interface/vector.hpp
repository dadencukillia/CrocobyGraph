#ifndef _CGRAPH_INTERFACE_VECTOR_HPP_
#define _CGRAPH_INTERFACE_VECTOR_HPP_

#include <cmath>

namespace CrocobyGraph {

  class Vector {
  public:
    float x { 0.0f };
    float y { 0.0f };

    Vector() = default;
    ~Vector() = default;

    constexpr explicit Vector(float angle) noexcept :
      x { std::cos(angle) },
      y { std::sin(angle) } {}

    constexpr Vector(float x, float y) noexcept :
      x { x },
      y { y } {}

    constexpr Vector(const Vector& another) noexcept :
      x { another.x },
      y { another.y } {}

    constexpr Vector& operator=(const Vector& another) noexcept = default;

    [[nodiscard]] constexpr float angle() const {
      return std::atan2(y, x);
    }

    [[nodiscard]] constexpr float length() const noexcept {
      return std::sqrt(x * x + y * y);
    }

    [[nodiscard]] constexpr Vector normalize() const {
      float len = length();
      return {
        x / len,
        y / len
      };
    }

    [[nodiscard]] constexpr Vector normalize_or(Vector fallback = { 0.0f, 0.0f }) const noexcept {
      float len = length();
      if (len == 0.0f) return fallback;

      return {
        x / len,
        y / len
      };
    }

    [[nodiscard]] constexpr float dot(Vector another) const noexcept {
      return x * another.x + y * another.y;
    }

    [[nodiscard]] constexpr float distance(Vector another) const noexcept {
      float d_x = x - another.x;
      float d_y = y - another.y;
      return std::sqrt(d_x * d_x + d_y * d_y);
    }

    [[nodiscard]] constexpr Vector operator-() const noexcept {
      return { -x, -y };
    }

    [[nodiscard]] constexpr Vector operator+(Vector another) const noexcept {
      return {
        x + another.x,
        y + another.y
      };
    }

    [[nodiscard]] constexpr Vector operator-(Vector another) const noexcept {
      return {
        x - another.x,
        y - another.y
      };
    }

    [[nodiscard]] constexpr Vector operator*(Vector another) const noexcept {
      return {
        x * another.x,
        y * another.y
      };
    }

    [[nodiscard]] constexpr Vector operator/(Vector another) const {
      return {
        x / another.x,
        y / another.y
      };
    }

    [[nodiscard]] constexpr Vector operator+(float v) const noexcept {
      return {
        x + v,
        y + v
      };
    }

    [[nodiscard]] constexpr Vector operator-(float v) const noexcept {
      return {
        x - v,
        y - v
      };
    }

    [[nodiscard]] constexpr Vector operator*(float v) const noexcept {
      return {
        x * v,
        y * v
      };
    }

    [[nodiscard]] constexpr Vector operator/(float v) const {
      float inv = 1.0f / v;
      return {
        x * inv,
        y * inv
      };
    }

    [[nodiscard]] constexpr Vector& operator+=(const Vector& another) noexcept {
      x += another.x;
      y += another.y;
      return *this;
    }

    [[nodiscard]] constexpr Vector& operator-=(const Vector& another) noexcept {
      x -= another.x;
      y -= another.y;
      return *this;
    }

    [[nodiscard]] constexpr Vector& operator*=(const Vector& another) noexcept {
      x *= another.x;
      y *= another.y;
      return *this;
    }

    [[nodiscard]] constexpr Vector& operator/=(const Vector& another) {
      x /= another.x;
      y /= another.y;
      return *this;
    }

    [[nodiscard]] constexpr Vector& operator+=(float v) noexcept {
      x += v;
      y += v;
      return *this;
    }

    [[nodiscard]] constexpr Vector& operator-=(float v) noexcept {
      x -= v;
      y -= v;
      return *this;
    }

    [[nodiscard]] constexpr Vector& operator*=(float v) noexcept {
      x *= v;
      y *= v;
      return *this;
    }

    [[nodiscard]] constexpr Vector& operator/=(float v) {
      float inv = 1.0f / v;
      x *= inv;
      y *= inv;
      return *this;
    }
  };

}

#endif
