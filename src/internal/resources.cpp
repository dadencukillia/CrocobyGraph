#include "resources.hpp"
#include <iostream>
#include "../resources/open_sans.hpp"
#include "../resources/circle_icons.hpp"
#include "../resources/media_controls_icons.hpp"
#include "raylib.h"

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

  StarCircleIcon::StarCircleIcon() {
    auto star_circle_image = LoadImageFromMemory(".png", StarCircleIconData, StarCircleIconSize);
    star_circle_icon = LoadTextureFromImage(star_circle_image);
    UnloadImage(star_circle_image);
  }
  StarCircleIcon::~StarCircleIcon() {
    UnloadTexture(star_circle_icon);
  }

  FlagCircleIcon::FlagCircleIcon() {
    auto flag_circle_image = LoadImageFromMemory(".png", FlagCircleIconData, FlagCircleIconSize);
    flag_circle_icon = LoadTextureFromImage(flag_circle_image);
    UnloadImage(flag_circle_image);
  }

  FlagCircleIcon::~FlagCircleIcon() {
    UnloadTexture(flag_circle_icon);
  };

  MediaControlIcons::MediaControlIcons() {
    auto arrow_back_image = LoadImageFromMemory(".png", ArrowBackIconData, ArrowBackIconSize);
    auto arrow_forward_image = LoadImageFromMemory(".png", ArrowForwardIconData, ArrowForwardIconSize);
    auto pause_image = LoadImageFromMemory(".png", PauseIconData, PauseIconSize);
    auto play_image = LoadImageFromMemory(".png", PlayIconData, PlayIconSize);

    back_icon = LoadTextureFromImage(arrow_back_image);
    forward_icon = LoadTextureFromImage(arrow_forward_image);
    pause_icon = LoadTextureFromImage(pause_image);
    play_icon = LoadTextureFromImage(play_image);

    UnloadImage(arrow_back_image);
    UnloadImage(arrow_forward_image);
    UnloadImage(pause_image);
    UnloadImage(play_image);
  }

  MediaControlIcons::~MediaControlIcons() {
    UnloadTexture(back_icon);
    UnloadTexture(forward_icon);
    UnloadTexture(pause_icon);
    UnloadTexture(play_icon);
  };

}
