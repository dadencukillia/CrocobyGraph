#ifndef _CGRAPH_INTERNAL_RESOURCES_HPP_
#define _CGRAPH_INTERNAL_RESOURCES_HPP_

#include "raylib.h"

namespace CrocobyGraph {

  struct FontResource {
    Font open_sans;

    FontResource();
    ~FontResource();
  };

  struct StarCircleIcon {
    Texture2D star_circle_icon;

    StarCircleIcon();
    ~StarCircleIcon();
  };

  struct FlagCircleIcon {
    Texture2D flag_circle_icon;

    FlagCircleIcon();
    ~FlagCircleIcon();
  };

  struct MediaControlIcons {
    Texture2D back_icon;
    Texture2D forward_icon;
    Texture2D pause_icon;
    Texture2D play_icon;

    MediaControlIcons();
    ~MediaControlIcons();
  };

}

#endif
