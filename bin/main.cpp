#include "crocobygraph/graphs.hpp"
#include "crocobygraph/ecs.hpp"
#include "crocobygraph/visual.hpp"

namespace cg = CrocobyGraph;

int main() {
  cg::Batch mb;

  auto n1 = mb.easy_node() | "First node" | cg::Vector { 0.0f, 0.0f } | cg::ColorPresets::RED;
  auto n2 = mb.easy_node() | cg::Vector { 50.0f, 50.0f };
  auto n3 = mb.easy_node() | cg::Vector { 200.0f, -100.0f };

  mb.add_label({
    .label = "Hey",
  }, n3);

  n1 == n2;
  n2 >> n3;
  n1 << n3;

  cg::GraphECS ecs {};
  auto map = ecs.get_scene().append(std::move(mb));

  ecs.get_scene().node_add_label(n2[map], "My text"); 

  ecs.add_system(cg::get_window_system<cg::PhysicsFrame, cg::EditorFrame>());
  ecs.run_loop();

  return 0;
}
