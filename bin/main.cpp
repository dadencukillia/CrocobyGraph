#include "crocobygraph/graphs.hpp"
#include "crocobygraph/ecs.hpp"
#include "crocobygraph/visual.hpp"
#include <iostream>

namespace cg = CrocobyGraph;
using namespace cg::aliases;

int main() {
  const LGN graphOne[] = {
    {
      .label = "Parent",
      .color = cg::Colors::RED,
      .points_to = { "child1", "child2" }
    },
    {
      .label = "Child 1",
      .id = "child1",
    },
    {
      .label = "Child 2",
      .id = "child2",
    }
  };

  std::string nodes[3] = { "Parent", "Child 1", "Child 2" };
  const bool connections[3][3] = {
    { false, false, true },
    { false, false, false },
    { false, false, false }
  };

  std::vector<LGN> graphTwo = cg::layout_from_adjacency_matrix<3>(
    nodes,
    connections
  );

  cg::Batch batch {};
  batch.add_node({ .position = { 10.0, 20.0 } }, { .label = "Node 1" });

  cg::GraphECS ecs {};
  ecs.get_scene()->append(std::move(batch));
  ecs.add_system(cg::get_window_system());
  ecs.run_loop();

  return 0;
}
