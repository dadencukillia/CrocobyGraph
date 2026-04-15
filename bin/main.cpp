#include "crocobygraph/graphs.hpp"
#include "crocobygraph/ecs.hpp"
#include "crocobygraph/visual.hpp"
#include <iostream>

namespace cg = CrocobyGraph;
using namespace cg::aliases;

int main() {
  std::vector<LGN> graphOne = {
    {
      .label = "Parent",
      .color = cg::ColorPresets::RED,
      .points_to = { "child1", "child2" }
    },
    {
      .label = "Child 1",
      .id = "child1",
    }
  };

  std::string nodes[3] = { "Parent", "Child 1", "Child 2" };
  const bool connections[3][3] = {
    { false, false, true },
    { false, false, false },
    { true, false, false }
  };

  std::vector<LGN> graphTwo = cg::layout_from_adjacency_matrix<3>(
    nodes,
    connections
  );

  cg::Batch decomposed { cg::decompose(std::move(graphTwo)) };

  cg::GraphECS ecs {};
  ecs.get_scene().append(std::move(decomposed));
  ecs.add_system(cg::get_window_system());
  ecs.run_loop();

  return 0;
}
