#include "color.hpp"
#include <string>
#include <format>

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) :
  r(r),
  g(g),
  b(b),
  a(alpha)
{}

Color::Color(uint32_t hex) :
  r((hex >> 24) & 0xFF),
  g((hex >> 16) & 0xFF),
  b((hex >> 8) & 0xFF),
  a(hex & 0xFF)
{}

uint8_t Color::get_red() const { return r; }
uint8_t Color::get_green() const { return g; }
uint8_t Color::get_blue() const { return b; }
uint8_t Color::get_alpha() const { return a; }

void Color::set_red(uint8_t red) {
  r = red;
}

void Color::set_green(uint8_t green) {
  g = green;
}
void Color::set_blue(uint8_t blue) {
  b = blue;
}

void Color::set_alpha(uint8_t alpha) {
  a = alpha;
}

std::string Color::to_hex_string() const {
  return std::format("{:02x}{:02x}{:02x}{:02x}", r, g, b, a);
}

std::string Color::to_rgba_string() const {
  return std::format("rgba({}, {}, {}, {})", r, g, b, a);
}

std::ostream& operator<<(std::ostream &in, const Color& color) {
  in << "#" << color.to_rgba_string();
  return in;
}
