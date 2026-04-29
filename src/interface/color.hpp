#ifndef _CGRAPH_INTERFACE_COLOR_HPP_
#define _CGRAPH_INTERFACE_COLOR_HPP_

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

    [[nodiscard]] uint8_t get_red() const;
    [[nodiscard]] uint8_t get_green() const;
    [[nodiscard]] uint8_t get_blue() const;
    [[nodiscard]] uint8_t get_alpha() const;

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
#undef WHITE
#undef BLACK
#undef RED
#undef GREEN
#undef BLUE
#undef YELLOW
#undef GOLD
#undef ORANGE
#undef PINK
#undef GRAY
#undef BROWN
#undef DARKGRAY
#undef LIGHTGRAY
#undef PURPLE
#undef VIOLET
#undef LIME
#undef MAROON
#undef DARKGREEN
#undef SKYBLUE
#undef DARKBLUE
#undef BEIGE
#undef MAGENTA
#undef DARKPURPLE
#undef DARKBROWN
#undef DARKRED

  // ===== Primary Colors =====
  inline constexpr Color WHITE       { 0xFFFFFFFF };
  inline constexpr Color BLACK       { 0x000000FF };
  inline constexpr Color RED         { 0xFF0000FF };
  inline constexpr Color GREEN       { 0x00FF00FF };
  inline constexpr Color BLUE        { 0x0000FFFF };

  // ===== Secondary Colors =====
  inline constexpr Color YELLOW      { 0xFFFF00FF };
  inline constexpr Color CYAN        { 0x00FFFFFF };
  inline constexpr Color MAGENTA     { 0xFF00FFFF };

  // ===== Warm Colors =====
  inline constexpr Color ORANGE      { 0xFF8000FF };
  inline constexpr Color GOLD        { 0xFFD700FF };
  inline constexpr Color CORAL       { 0xFF7F50FF };
  inline constexpr Color SALMON      { 0xFA8072FF };
  inline constexpr Color TOMATO      { 0xFF6347FF };
  inline constexpr Color ORANGERED   { 0xFF4500FF };

  // ===== Cool Colors =====
  inline constexpr Color SKYBLUE     { 0x87CEEBFF };
  inline constexpr Color LIGHTBLUE   { 0xADD8E6FF };
  inline constexpr Color DARKBLUE    { 0x00008BFF };
  inline constexpr Color NAVY        { 0x000080FF };
  inline constexpr Color TEAL        { 0x008080FF };
  inline constexpr Color TURQUOISE   { 0x40E0D0FF };
  inline constexpr Color AQUA        { 0x00FFFFFF };
  inline constexpr Color STEELBLUE   { 0x4682B4FF };

  // ===== Green Shades =====
  inline constexpr Color LIME        { 0x00FF00FF };
  inline constexpr Color LIMEGREEN   { 0x32CD32FF };
  inline constexpr Color LIGHTGREEN  { 0x90EE90FF };
  inline constexpr Color MEDIUMGREEN { 0x00A651FF };
  inline constexpr Color DARKGREEN   { 0x006400FF };
  inline constexpr Color FORESTGREEN { 0x228B22FF };
  inline constexpr Color SEAGREEN    { 0x2E8B57FF };
  inline constexpr Color OLIVE       { 0x808000FF };

  // ===== Purple/Violet Shades =====
  inline constexpr Color PURPLE      { 0x800080FF };
  inline constexpr Color VIOLET      { 0xEE82EEFF };
  inline constexpr Color INDIGO      { 0x4B0082FF };
  inline constexpr Color PLUM        { 0xDDA0DDFF };
  inline constexpr Color ORCHID      { 0xDA70D6FF };
  inline constexpr Color MEDIUMVIOLET { 0xC71585FF };
  inline constexpr Color MEDIUMPURPLE { 0x9370DBFF };
  inline constexpr Color DARKVIOLET  { 0x9400D3FF };
  inline constexpr Color BLUEVIOLET  { 0x8A2BE2FF };
  inline constexpr Color DARKMAGENTA { 0x8B008BFF };

  // ===== Pink/Red Shades =====
  inline constexpr Color PINK        { 0xFFC0CBFF };
  inline constexpr Color LIGHTPINK   { 0xFFB6C1FF };
  inline constexpr Color HOTPINK     { 0xFF69B4FF };
  inline constexpr Color DEEPPINK    { 0xFF1493FF };
  inline constexpr Color CRIMSON     { 0xDC143CFF };
  inline constexpr Color DARKRED     { 0x8B0000FF };
  inline constexpr Color MAROON      { 0x800000FF };
  inline constexpr Color FIREBRICK   { 0xB22222FF };
  inline constexpr Color INDIANRED   { 0xCD5C5CFF };
  inline constexpr Color LIGHTCORAL  { 0xF08080FF };

  // ===== Brown/Earth Tones =====
  inline constexpr Color BROWN       { 0x8B4513FF };
  inline constexpr Color DARKBROWN   { 0x654321FF };
  inline constexpr Color SADLEBROWN  { 0x8B4513FF };
  inline constexpr Color SIENNA      { 0xA0522DFF };
  inline constexpr Color SADDLEBROWN { 0x8B4513FF };
  inline constexpr Color PERU        { 0xCD853FFF };
  inline constexpr Color ROSYBROWN   { 0xBC8F8FFF };
  inline constexpr Color BURLYWOOD   { 0xDEB887FF };
  inline constexpr Color SANDYBROWN  { 0xF4A460FF };
  inline constexpr Color GOLDENROD   { 0xDAA520FF };
  inline constexpr Color CHOCOLATE   { 0xD2691EFF };
  inline constexpr Color BEIGE       { 0xF5F5DCFF };
  inline constexpr Color TAN         { 0xD2B48CFF };
  inline constexpr Color KHAKI       { 0xF0E68CFF };

  // ===== Gray/Silver Shades =====
  inline constexpr Color LIGHTGRAY   { 0xD3D3D3FF };
  inline constexpr Color GAINSBORO   { 0xDCDCDCFF };
  inline constexpr Color WHITESMOKE  { 0xF5F5F5FF };
  inline constexpr Color GRAY        { 0x808080FF };
  inline constexpr Color DARKGRAY    { 0xA9A9A9FF };
  inline constexpr Color DIMGRAY     { 0x696969FF };
  inline constexpr Color SILVER      { 0xC0C0C0FF };
  inline constexpr Color SLATEGRAY   { 0x708090FF };
  inline constexpr Color DARKSLATEGRAY { 0x2F4F4FFF };
  inline constexpr Color LIGHTSLATEGRAY { 0x778899FF };

}

}

#endif
