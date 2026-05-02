#include "crocobygraph/graphs.hpp"
#include "crocobygraph/ecs.hpp"
#include "crocobygraph/visual.hpp"

namespace cg = CrocobyGraph;

int main() {
  cg::Batch mb;

  auto node1 = mb.easy_node() | "Node 1" | cg::Vector{0.0f, 0.0f};
  auto node2 = mb.easy_node() | "Node 2" | cg::Vector{100.0f, 0.0f};

  node1 >> node2;

  cg::GraphECS ecs{};
  auto map = ecs.get_scene().append(std::move(mb));

  // Get real entt::entity IDs using the map
  auto real_entity1 = node1[map];
  auto real_entity2 = node2[map];

  // Runtime updates
  ecs.get_scene().set_pos(real_entity1, cg::Vector{50.0f, 50.0f});
  ecs.get_scene().set_color(real_entity2, cg::ColorPresets::BLUE);
  
  // Add labels
  ecs.get_scene().node_set_label(real_entity1, "Updated");

  ecs.add_system(cg::get_window_system<cg::BFSFrame, cg::EditorFrame>());
  ecs.run_loop();

  return 0;
}
