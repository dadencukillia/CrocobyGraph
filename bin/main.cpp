#include "layout.hpp"
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

  std::vector<LGN> graphTwo = cg::layoutFromAdjacencyMatrix<3>(
    nodes,
    connections
  );

  std::cout << "Wait for features!" << '\n';

  return 0;
}
