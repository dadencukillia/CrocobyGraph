#include <cstdint>
#include <ostream>
#include <string>

class Color {
  uint8_t r { 0 };
  uint8_t g { 0 };
  uint8_t b { 0 };
  uint8_t a { 255 };

public:
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255);
  Color(uint32_t hex);

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

  friend std::ostream& operator<<(std::ostream &in, const Color& color);
};

namespace Colors {
  const Color WHITE { 0xFFFFFFFF };
  const Color BLACK { 0x000000FF };
  const Color RED { 0xFF0000FF };
  const Color GREEN { 0x00FF00FF };
  const Color BLUE { 0x0000FFFF };
  const Color TRANSPARENT { 0 };
}
