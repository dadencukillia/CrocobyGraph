#include "resources.hpp"
#include <iostream>
#include "../resources/open_sans.hpp"

namespace CrocobyGraph {

  FontResource::FontResource() {
    open_sans = LoadFontFromMemory(".ttf", OpenSansFontData, OpenSansFontSize, 96, nullptr, 0);
    SetTextureFilter(open_sans.texture, TEXTURE_FILTER_BILINEAR);
    std::cout << "Open Sans font loaded\n";
  }

  FontResource::~FontResource() {
    UnloadFont(open_sans);
    std::cout << "Open Sans font unloaded\n";
  }

}
