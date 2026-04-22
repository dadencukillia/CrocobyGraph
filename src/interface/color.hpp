#ifndef _CGRAPH_COLOR_HPP_
#define _CGRAPH_COLOR_HPP_

#include <cstdint>
#include <ostream>
#include <string>

extern "C" {
  struct Color;
}

namespace CrocobyGraph {

  class Color {
    uint8_t r { 0 };
    uint8_t g { 0 };
    uint8_t b { 0 };
    uint8_t a { 255 };

  public:
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255) :
      r(r), g(g), b(b), a(alpha) {}

    constexpr Color(uint32_t hex) :
      r((hex >> 24) & 0xFF),
      g((hex >> 16) & 0xFF),
      b((hex >> 8) & 0xFF),
      a(hex & 0xFF) {}

    uint8_t get_red() const;
    uint8_t get_green() const;
    uint8_t get_blue() const;
    uint8_t get_alpha() const;

    void set_red(uint8_t);
    void set_green(uint8_t);
    void set_blue(uint8_t);
    void set_alpha(uint8_t);

    std::string to_hex_string() const;
    std::string to_rgba_string() const;

    operator ::Color() const;

    friend std::ostream& operator<<(std::ostream &in, const Color& color);
  };

  namespace ColorPresets {
    inline const Color WHITE(0xFFFFFFFF);
    inline const Color BLACK(0x000000FF);
    inline const Color RED(0xFF0000FF);
    inline const Color GREEN(0x00FF00FF);
    inline const Color BLUE(0x0000FFFF);
    inline const Color TRANSPARENT(0);
  }

}

#endif
