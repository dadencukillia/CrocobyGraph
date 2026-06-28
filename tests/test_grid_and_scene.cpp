// Tests for:
//   - generate_grid()          (src/interface/grid.hpp/.cpp)
//   - Scene::adjacency_matrix() (src/interface/scene.hpp/.cpp)
//
// Requires linking against the CrocobyGraph library (entt, etc.) but does
// NOT open a window / touch GPU resources.

#include "test_utils.hpp"
#include "../src/interface/grid.hpp"
#include "../src/interface/scene.hpp"
#include "../src/interface/batch.hpp"
#include "../src/interface/adjacency_matrix.hpp"
#include "../src/interface/entities.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

using CrocobyGraph::AdjacencyMatrix;
using CrocobyGraph::Batch;
using CrocobyGraph::Scene;
using CrocobyGraph::generate_grid;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static float approx_eq(float a, float b, float eps = 0.001f) {
  return std::fabs(a - b) < eps;
}

// ---------------------------------------------------------------------------
// generate_grid: node & edge counts
// ---------------------------------------------------------------------------

TEST_CASE(grid_1x1_has_one_node_no_edges) {
  auto batch = generate_grid(1, 1);
  Scene scene;
  scene.append(std::move(batch));
  auto ns = scene.nodes();
  CHECK_EQ(ns.size(), 1u);
  // No edges in a 1×1 grid
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 1u);
  CHECK(matrix.at(0, 0) == false);
}

TEST_CASE(grid_2x1_has_two_nodes_one_edge) {
  auto batch = generate_grid(2, 1);
  Scene scene;
  scene.append(std::move(batch));
  CHECK_EQ(scene.nodes().size(), 2u);
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 2u);
  // Exactly one undirected edge => both directions true
  int edge_count = (matrix.at(0, 1) ? 1 : 0) + (matrix.at(1, 0) ? 1 : 0);
  CHECK_EQ(edge_count, 2);
}

TEST_CASE(grid_1x2_has_two_nodes_one_edge) {
  auto batch = generate_grid(1, 2);
  Scene scene;
  scene.append(std::move(batch));
  CHECK_EQ(scene.nodes().size(), 2u);
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 2u);
  int edge_count = (matrix.at(0, 1) ? 1 : 0) + (matrix.at(1, 0) ? 1 : 0);
  CHECK_EQ(edge_count, 2);
}

TEST_CASE(grid_2x2_has_four_nodes_four_edges) {
  // 2×2 grid: 4 nodes, 4 undirected edges (2 horizontal + 2 vertical)
  auto batch = generate_grid(2, 2);
  Scene scene;
  scene.append(std::move(batch));
  CHECK_EQ(scene.nodes().size(), 4u);

  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 4u);

  // Count total directed entries (each undirected edge contributes 2)
  int total = 0;
  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 4; ++j)
      if (matrix.at(i, j)) ++total;
  // 4 undirected edges => 8 directed entries in boolean matrix
  CHECK_EQ(total, 8);
}

TEST_CASE(grid_3x2_node_count) {
  // 3 wide, 2 tall => 6 nodes
  auto batch = generate_grid(3, 2);
  Scene scene;
  scene.append(std::move(batch));
  CHECK_EQ(scene.nodes().size(), 6u);
}

TEST_CASE(grid_2x3_node_count) {
  auto batch = generate_grid(2, 3);
  Scene scene;
  scene.append(std::move(batch));
  CHECK_EQ(scene.nodes().size(), 6u);
}

TEST_CASE(grid_3x3_node_and_edge_count) {
  // 3×3: 9 nodes
  // edges: 3*(3-1) horizontal + 3*(3-1) vertical = 6+6 = 12 undirected edges
  auto batch = generate_grid(3, 3);
  Scene scene;
  scene.append(std::move(batch));
  CHECK_EQ(scene.nodes().size(), 9u);

  auto [nodes, matrix] = scene.adjacency_matrix();
  int total = 0;
  for (size_t i = 0; i < 9; ++i)
    for (size_t j = 0; j < 9; ++j)
      if (matrix.at(i, j)) ++total;
  // 12 undirected edges => 24 directed entries
  CHECK_EQ(total, 24);
}

// ---------------------------------------------------------------------------
// generate_grid: positions & spacing
// ---------------------------------------------------------------------------

TEST_CASE(grid_1x1_default_spacing_at_origin) {
  auto batch = generate_grid(1, 1);
  Scene scene;
  auto ids = scene.append(std::move(batch));
  // Single node: offset_x = (1-1)*60*0.5 = 0, offset_y = 0
  auto pos = scene.pos(ids[0]);
  CHECK(approx_eq(pos.x, 0.0f));
  CHECK(approx_eq(pos.y, 0.0f));
}

TEST_CASE(grid_2x1_spacing) {
  const float spacing = 60.0f;
  auto batch = generate_grid(2, 1, spacing);
  Scene scene;
  auto ids = scene.append(std::move(batch));
  // offset_x = (1-2)*60*0.5 = -30
  // node0: x=-30, node1: x=30; y=0 for both
  CHECK_EQ(scene.nodes().size(), 2u);
  // Verify the two node positions are spacing apart along x
  auto p0 = scene.pos(ids[0]);
  auto p1 = scene.pos(ids[1]);
  CHECK(approx_eq(std::fabs(p0.x - p1.x), spacing));
  CHECK(approx_eq(p0.y, p1.y));
}

TEST_CASE(grid_1x2_spacing) {
  const float spacing = 60.0f;
  auto batch = generate_grid(1, 2, spacing);
  Scene scene;
  auto ids = scene.append(std::move(batch));
  auto p0 = scene.pos(ids[0]);
  auto p1 = scene.pos(ids[1]);
  CHECK(approx_eq(p0.x, p1.x));
  CHECK(approx_eq(std::fabs(p0.y - p1.y), spacing));
}

TEST_CASE(grid_custom_spacing) {
  const float spacing = 100.0f;
  auto batch = generate_grid(2, 2, spacing);
  Scene scene;
  auto ids = scene.append(std::move(batch));
  // Positions: offset_x = (1-2)*100*0.5 = -50, offset_y = -50
  // (0,0)->(-50,-50), (1,0)->(50,-50), (0,1)->(-50,50), (1,1)->(50,50)
  // Check that adjacent nodes differ by exactly `spacing`
  auto p0 = scene.pos(ids[0]); // node(0,0)
  auto p1 = scene.pos(ids[1]); // node(1,0) - horizontal neighbour
  CHECK(approx_eq(std::fabs(p0.x - p1.x), spacing));
  CHECK(approx_eq(std::fabs(p0.y - p1.y), 0.0f));
}

TEST_CASE(grid_default_spacing_is_60) {
  auto batch_default = generate_grid(2, 1);
  auto batch_60 = generate_grid(2, 1, 60.0f);
  Scene s1, s2;
  auto ids1 = s1.append(std::move(batch_default));
  auto ids2 = s2.append(std::move(batch_60));
  auto p1a = s1.pos(ids1[0]);
  auto p1b = s1.pos(ids1[1]);
  auto p2a = s2.pos(ids2[0]);
  auto p2b = s2.pos(ids2[1]);
  CHECK(approx_eq(p1a.x, p2a.x));
  CHECK(approx_eq(p1b.x, p2b.x));
}

// ---------------------------------------------------------------------------
// generate_grid: undirected edges (both directions reachable)
// ---------------------------------------------------------------------------

TEST_CASE(grid_edges_are_undirected) {
  auto batch = generate_grid(2, 2);
  Scene scene;
  scene.append(std::move(batch));
  auto [nodes, matrix] = scene.adjacency_matrix();
  // For every i,j: matrix[i][j] == matrix[j][i]
  for (size_t i = 0; i < nodes.size(); ++i)
    for (size_t j = 0; j < nodes.size(); ++j)
      CHECK(matrix.at(i, j) == matrix.at(j, i));
}

TEST_CASE(grid_no_self_loops) {
  auto batch = generate_grid(3, 3);
  Scene scene;
  scene.append(std::move(batch));
  auto [nodes, matrix] = scene.adjacency_matrix();
  for (size_t i = 0; i < nodes.size(); ++i)
    CHECK(matrix.at(i, i) == false);
}

// ---------------------------------------------------------------------------
// Scene::adjacency_matrix(): empty scene
// ---------------------------------------------------------------------------

TEST_CASE(scene_adjacency_empty_scene) {
  Scene scene;
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 0u);
  CHECK_EQ(matrix.size(), 0u);
}

// ---------------------------------------------------------------------------
// Scene::adjacency_matrix(): undirected edges
// ---------------------------------------------------------------------------

TEST_CASE(scene_adjacency_two_nodes_undirected) {
  Batch b;
  auto n0 = b.add_node({});
  auto n1 = b.add_node({});
  b.add_edge({ .node_start = n0, .node_end = n1 }); // undirected (no arrows)

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 2u);
  CHECK_EQ(matrix.size(), 2u);

  // Undirected: both directions should be set
  size_t trues = 0;
  for (size_t i = 0; i < 2; ++i)
    for (size_t j = 0; j < 2; ++j)
      if (matrix.at(i, j)) ++trues;
  CHECK_EQ(trues, 2u);
}

TEST_CASE(scene_adjacency_undirected_is_symmetric) {
  Batch b;
  auto n0 = b.add_node({});
  auto n1 = b.add_node({});
  auto n2 = b.add_node({});
  b.add_edge({ .node_start = n0, .node_end = n1 });
  b.add_edge({ .node_start = n1, .node_end = n2 });

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 3u);
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      CHECK(matrix.at(i, j) == matrix.at(j, i));
}

TEST_CASE(scene_adjacency_disconnected_nodes_have_no_edges) {
  Batch b;
  b.add_node({});
  b.add_node({});
  // No edges added

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 2u);
  CHECK(matrix.at(0, 0) == false);
  CHECK(matrix.at(0, 1) == false);
  CHECK(matrix.at(1, 0) == false);
  CHECK(matrix.at(1, 1) == false);
}

// ---------------------------------------------------------------------------
// Scene::adjacency_matrix(): directed edges (arrows)
// ---------------------------------------------------------------------------

TEST_CASE(scene_adjacency_arrow_on_end_directed) {
  // arrow_on_end means edge points from start->end
  Batch b;
  auto n0 = b.add_node({});
  auto n1 = b.add_node({});
  b.add_edge({ .node_start = n0, .node_end = n1, .arrow_on_end = true });

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 2u);

  // Find indices for n0 and n1 in sorted nodes
  // nodes is sorted, so we compare matrix entries: exactly one of (0,1) or (1,0) is true
  int a = (matrix.at(0, 1) ? 1 : 0) + (matrix.at(1, 0) ? 1 : 0);
  CHECK_EQ(a, 1); // Only one direction
}

TEST_CASE(scene_adjacency_arrow_on_start_directed) {
  // arrow_on_start means the edge points from end->start
  Batch b;
  auto n0 = b.add_node({});
  auto n1 = b.add_node({});
  b.add_edge({ .node_start = n0, .node_end = n1, .arrow_on_start = true });

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 2u);

  int a = (matrix.at(0, 1) ? 1 : 0) + (matrix.at(1, 0) ? 1 : 0);
  CHECK_EQ(a, 1); // Only one direction
}

TEST_CASE(scene_adjacency_arrow_both_ends_is_undirected_equivalent) {
  // arrow_on_start && arrow_on_end => directed is true, but both directions set
  Batch b;
  auto n0 = b.add_node({});
  auto n1 = b.add_node({});
  b.add_edge({ .node_start = n0, .node_end = n1, .arrow_on_start = true, .arrow_on_end = true });

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  int a = (matrix.at(0, 1) ? 1 : 0) + (matrix.at(1, 0) ? 1 : 0);
  CHECK_EQ(a, 2); // Both directions
}

TEST_CASE(scene_adjacency_no_self_loops_from_normal_edges) {
  Batch b;
  auto n0 = b.add_node({});
  auto n1 = b.add_node({});
  b.add_edge({ .node_start = n0, .node_end = n1 });

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  for (size_t i = 0; i < 2; ++i)
    CHECK(matrix.at(i, i) == false);
}

// ---------------------------------------------------------------------------
// Scene::adjacency_matrix(): multiple edges
// ---------------------------------------------------------------------------

TEST_CASE(scene_adjacency_three_nodes_triangle) {
  Batch b;
  auto n0 = b.add_node({});
  auto n1 = b.add_node({});
  auto n2 = b.add_node({});
  b.add_edge({ .node_start = n0, .node_end = n1 });
  b.add_edge({ .node_start = n1, .node_end = n2 });
  b.add_edge({ .node_start = n2, .node_end = n0 });

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 3u);

  // 3 undirected edges => 6 true entries
  int total = 0;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      if (matrix.at(i, j)) ++total;
  CHECK_EQ(total, 6);
}

TEST_CASE(scene_adjacency_star_graph) {
  // Star: center node connected to 3 leaves
  Batch b;
  auto center = b.add_node({});
  auto l0 = b.add_node({});
  auto l1 = b.add_node({});
  auto l2 = b.add_node({});
  b.add_edge({ .node_start = center, .node_end = l0 });
  b.add_edge({ .node_start = center, .node_end = l1 });
  b.add_edge({ .node_start = center, .node_end = l2 });

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 4u);

  // Each leaf connects to center => 3 undirected edges => 6 true entries
  int total = 0;
  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 4; ++j)
      if (matrix.at(i, j)) ++total;
  CHECK_EQ(total, 6);

  // Center (whichever index) should have 3 connections
  // Find center index by finding the node with 3 true entries in its row
  bool found_center = false;
  for (size_t i = 0; i < 4; ++i) {
    int cnt = 0;
    for (size_t j = 0; j < 4; ++j)
      if (matrix.at(i, j)) ++cnt;
    if (cnt == 3) { found_center = true; break; }
  }
  CHECK(found_center);
}

TEST_CASE(scene_adjacency_returns_sorted_nodes_vector) {
  Batch b;
  b.add_node({});
  b.add_node({});
  b.add_node({});

  Scene scene;
  scene.append(std::move(b));
  auto [nodes, matrix] = scene.adjacency_matrix();
  CHECK_EQ(nodes.size(), 3u);
  // nodes should be sorted
  for (size_t i = 1; i < nodes.size(); ++i)
    CHECK(nodes[i - 1] < nodes[i]);
}

int main() {
  return TestUtils::report_and_exit();
}