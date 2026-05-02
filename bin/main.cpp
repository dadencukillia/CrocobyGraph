#include "crocobygraph/graphs.hpp"
#include "crocobygraph/ecs.hpp"
#include "crocobygraph/visual.hpp"

namespace cg = CrocobyGraph;

int main() {
  cg::GraphECS ecs{};
  ecs.get_scene() += cg::generate_grid(30, 30);
  ecs.add_system(cg::get_window_system<cg::BFSFrame, cg::EditorFrame>());
  ecs.run_loop();

  return 0;
}
