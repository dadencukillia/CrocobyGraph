#ifndef _CGRAPH_RESOURCES_HPP_
#define _CGRAPH_RESOURCES_HPP_

#include "raylib.h"

namespace CrocobyGraph {

  struct FontResource {
    Font open_sans;

    FontResource();
    ~FontResource();
  };

}

#endif
