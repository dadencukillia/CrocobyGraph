#include "crocobygraph/graphs.hpp"
#include "crocobygraph/ecs.hpp"
#include "crocobygraph/visual.hpp"

namespace cg = CrocobyGraph;

int main() {
  cg::GraphECS ecs{};
  ecs.add_system(cg::get_window_system<cg::BFSFrame, cg::EditorFrame, cg::PhysicsFrame>());
  ecs.run_loop();

  return 0;
}
