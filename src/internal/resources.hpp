#ifndef _CGRAPH_INTERNAL_RESOURCES_HPP_
#define _CGRAPH_INTERNAL_RESOURCES_HPP_

#include "raylib.h"

namespace CrocobyGraph {

  struct FontResource {
    Font open_sans;

    FontResource();
    ~FontResource();
  };

}

#endif
