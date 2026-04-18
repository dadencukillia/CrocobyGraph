#include "crocobygraph/graphs.hpp"
#include "crocobygraph/ecs.hpp"
#include "crocobygraph/visual.hpp"
#include <iostream>

namespace cg = CrocobyGraph;
using namespace cg::aliases;

int main() {
  std::vector<LGN> graphOne = {
    {
      .label = "Start",
      .color = cg::ColorPresets::RED,
      .points_to = { "finish" },
      .id = "start",
    },
    {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
    {
      .label = "Finish",
      .color = { 0x00FF00FF },
      .points_to = { "start", "finish" },
      .id = "finish",
    },
  };

  std::string nodes[3] = { "Parent", "Child 1", "Child 2" };
  const bool connections[3][3] = {
    { true, true, true },
    { false, true, false },
    { false, false, true }
  };

  std::vector<LGN> graphTwo = cg::layout_from_adjacency_matrix<3>(
    std::move(nodes),
    connections
  );

  cg::Batch decomposed { cg::decompose(std::move(graphOne)) };
  cg::Batch neural_network { cg::neural_network({
    "Age",
    "Room",
    "Gender"
  }, { 10, 5, 5, 5, 5, 5, 5, 5 }, {
    "Chance"
  }) };

  cg::GraphECS ecs {};
  ecs.get_scene().append(std::move(decomposed));
  ecs.add_system(cg::get_window_system<cg::PhysicsFrame, cg::EditorFrame>());
  ecs.run_loop();

  return 0;
}
