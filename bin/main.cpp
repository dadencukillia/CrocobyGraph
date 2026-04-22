#include "crocobygraph/graphs.hpp"
#include "crocobygraph/ecs.hpp"
#include "crocobygraph/visual.hpp"

namespace cg = CrocobyGraph;

int main() {
  std::vector<cg::LayoutGraphNode> graphOne = {
    {
      .label = "Start",
      .color = cg::ColorPresets::RED,
      .points_to = { "finish" },
      .id = "start",
    },
    {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
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

  std::vector<cg::LayoutGraphNode> graphTwo = cg::layout_from_adjacency_matrix<3>(
    std::move(nodes),
    connections
  );

  cg::Batch decomposed { cg::decompose(std::move(graphOne)) };
  cg::Batch neural_network { cg::neural_network({
    "Ticket class",
    "Gender",
    "Age",
    "Parch",
    "Fare"
  }, { 32, 16 }, {
    "Survivance chance"
  }) };

  cg::GraphECS ecs {};
  ecs.get_scene().append(std::move(neural_network));
  ecs.add_system(cg::get_window_system<cg::PhysicsFrame, cg::EditorFrame>());
  ecs.run_loop();

  return 0;
}
