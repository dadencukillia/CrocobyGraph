// Tests for AdjacencyMatrix (src/interface/adjacency_matrix.hpp/.cpp)
// This file only depends on adjacency_matrix.cpp + stdlib (no raylib/imgui needed).

#include "test_utils.hpp"
#include "../src/interface/adjacency_matrix.hpp"
#include <stdexcept>

using CrocobyGraph::AdjacencyMatrix;

// ---------------------------------------------------------------------------
// Construction and basic accessors
// ---------------------------------------------------------------------------

TEST_CASE(constructor_zero_nodes) {
  AdjacencyMatrix m{0};
  CHECK_EQ(m.size(), 0u);
}

TEST_CASE(constructor_single_node) {
  AdjacencyMatrix m{1};
  CHECK_EQ(m.size(), 1u);
  CHECK(m.at(0, 0) == false);
}

TEST_CASE(constructor_initialises_all_false) {
  AdjacencyMatrix m{4};
  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 4; ++j)
      CHECK(m.at(i, j) == false);
}

// ---------------------------------------------------------------------------
// set / at
// ---------------------------------------------------------------------------

TEST_CASE(set_and_read_single_edge) {
  AdjacencyMatrix m{3};
  m.set(0, 1, true);
  CHECK(m.at(0, 1) == true);
  CHECK(m.at(1, 0) == false); // undirected not implied
  CHECK(m.at(0, 0) == false);
}

TEST_CASE(set_true_then_false) {
  AdjacencyMatrix m{2};
  m.set(0, 1, true);
  m.set(0, 1, false);
  CHECK(m.at(0, 1) == false);
}

TEST_CASE(set_all_edges_in_2x2) {
  AdjacencyMatrix m{2};
  m.set(0, 0, true);
  m.set(0, 1, true);
  m.set(1, 0, true);
  m.set(1, 1, true);
  CHECK(m.at(0, 0) == true);
  CHECK(m.at(0, 1) == true);
  CHECK(m.at(1, 0) == true);
  CHECK(m.at(1, 1) == true);
}

TEST_CASE(set_self_loop) {
  AdjacencyMatrix m{3};
  m.set(2, 2, true);
  CHECK(m.at(2, 2) == true);
  CHECK(m.at(0, 0) == false);
  CHECK(m.at(1, 1) == false);
}

// ---------------------------------------------------------------------------
// operator==
// ---------------------------------------------------------------------------

TEST_CASE(equality_empty_matrices) {
  AdjacencyMatrix a{3};
  AdjacencyMatrix b{3};
  CHECK(a == b);
}

TEST_CASE(equality_after_same_set) {
  AdjacencyMatrix a{3};
  AdjacencyMatrix b{3};
  a.set(0, 2, true);
  b.set(0, 2, true);
  CHECK(a == b);
}

TEST_CASE(inequality_different_edges) {
  AdjacencyMatrix a{3};
  AdjacencyMatrix b{3};
  a.set(0, 1, true);
  b.set(1, 2, true);
  CHECK(!(a == b));
}

// ---------------------------------------------------------------------------
// Copy constructor & assignment
// ---------------------------------------------------------------------------

TEST_CASE(copy_constructor) {
  AdjacencyMatrix orig{3};
  orig.set(1, 2, true);
  AdjacencyMatrix copy{orig};
  CHECK(copy == orig);
  CHECK_EQ(copy.size(), 3u);
  // Mutation of copy does not affect original
  copy.set(0, 0, true);
  CHECK(!(copy == orig));
  CHECK(orig.at(0, 0) == false);
}

TEST_CASE(copy_assignment) {
  AdjacencyMatrix a{3};
  a.set(0, 1, true);
  AdjacencyMatrix b{2};
  b = a;
  CHECK(b == a);
  CHECK_EQ(b.size(), 3u);
}

TEST_CASE(self_assignment) {
  AdjacencyMatrix m{2};
  m.set(0, 1, true);
  AdjacencyMatrix& ref = m;
  ref = ref; // self-assignment
  CHECK(m.at(0, 1) == true);
  CHECK_EQ(m.size(), 2u);
}

// ---------------------------------------------------------------------------
// transpose()
// ---------------------------------------------------------------------------

TEST_CASE(transpose_zero_matrix) {
  AdjacencyMatrix m{3};
  AdjacencyMatrix t = m.transpose();
  CHECK(t == m);
  CHECK_EQ(t.size(), 3u);
}

TEST_CASE(transpose_single_directed_edge) {
  // m: 0->1 exists, 1->0 does not
  AdjacencyMatrix m{3};
  m.set(0, 1, true);
  AdjacencyMatrix t = m.transpose();
  CHECK(t.at(1, 0) == true);
  CHECK(t.at(0, 1) == false);
}

TEST_CASE(transpose_symmetric_is_identity) {
  AdjacencyMatrix m{3};
  m.set(0, 1, true);
  m.set(1, 0, true);
  m.set(0, 2, true);
  m.set(2, 0, true);
  AdjacencyMatrix t = m.transpose();
  CHECK(t == m);
}

TEST_CASE(transpose_double_transpose_equals_original) {
  AdjacencyMatrix m{4};
  m.set(0, 1, true);
  m.set(1, 3, true);
  m.set(2, 0, true);
  AdjacencyMatrix tt = m.transpose().transpose();
  CHECK(tt == m);
}

TEST_CASE(transpose_full_upper_triangle) {
  // Set upper triangle; transpose should produce lower triangle
  AdjacencyMatrix m{3};
  m.set(0, 1, true);
  m.set(0, 2, true);
  m.set(1, 2, true);
  AdjacencyMatrix t = m.transpose();
  CHECK(t.at(1, 0) == true);
  CHECK(t.at(2, 0) == true);
  CHECK(t.at(2, 1) == true);
  CHECK(t.at(0, 1) == false);
  CHECK(t.at(0, 2) == false);
  CHECK(t.at(1, 2) == false);
}

TEST_CASE(transpose_preserves_self_loops) {
  AdjacencyMatrix m{3};
  m.set(1, 1, true);
  AdjacencyMatrix t = m.transpose();
  CHECK(t.at(1, 1) == true);
  CHECK(t.at(0, 0) == false);
  CHECK(t.at(2, 2) == false);
}

// ---------------------------------------------------------------------------
// operator+ (logical OR of edge sets)
// ---------------------------------------------------------------------------

TEST_CASE(plus_combines_disjoint_edges) {
  AdjacencyMatrix a{3};
  a.set(0, 1, true);
  AdjacencyMatrix b{3};
  b.set(1, 2, true);
  AdjacencyMatrix c = a + b;
  CHECK(c.at(0, 1) == true);
  CHECK(c.at(1, 2) == true);
  CHECK(c.at(0, 2) == false);
}

TEST_CASE(plus_overlapping_edges_is_true) {
  AdjacencyMatrix a{2};
  a.set(0, 1, true);
  AdjacencyMatrix b{2};
  b.set(0, 1, true);
  AdjacencyMatrix c = a + b;
  CHECK(c.at(0, 1) == true);
}

TEST_CASE(plus_empty_matrices_stays_empty) {
  AdjacencyMatrix a{3};
  AdjacencyMatrix b{3};
  AdjacencyMatrix c = a + b;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      CHECK(c.at(i, j) == false);
}

TEST_CASE(plus_size_mismatch_throws) {
  AdjacencyMatrix a{2};
  AdjacencyMatrix b{3};
  CHECK_THROWS(a + b);
}

TEST_CASE(plus_does_not_modify_operands) {
  AdjacencyMatrix a{2};
  a.set(0, 1, true);
  AdjacencyMatrix b{2};
  b.set(1, 0, true);
  AdjacencyMatrix orig_a{a};
  AdjacencyMatrix orig_b{b};
  (void)(a + b);
  CHECK(a == orig_a);
  CHECK(b == orig_b);
}

// ---------------------------------------------------------------------------
// operator* (boolean matrix multiplication / reachability in two steps)
// ---------------------------------------------------------------------------

TEST_CASE(multiply_zero_matrices_stays_zero) {
  AdjacencyMatrix a{3};
  AdjacencyMatrix b{3};
  AdjacencyMatrix c = a * b;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      CHECK(c.at(i, j) == false);
}

TEST_CASE(multiply_size_mismatch_throws) {
  AdjacencyMatrix a{2};
  AdjacencyMatrix b{3};
  CHECK_THROWS(a * b);
}

TEST_CASE(multiply_two_step_path) {
  // 0->1->2  =>  A*A should have 0->2
  AdjacencyMatrix m{3};
  m.set(0, 1, true);
  m.set(1, 2, true);
  AdjacencyMatrix mm = m * m;
  CHECK(mm.at(0, 2) == true);
  CHECK(mm.at(0, 1) == false); // no direct 2-step 0->1
  CHECK(mm.at(1, 2) == false); // no 2-step 1->2
}

TEST_CASE(multiply_identity_matrix_unchanged) {
  // Build identity: I[i][i] = true
  AdjacencyMatrix ident{3};
  ident.set(0, 0, true);
  ident.set(1, 1, true);
  ident.set(2, 2, true);

  AdjacencyMatrix m{3};
  m.set(0, 1, true);
  m.set(2, 0, true);

  // I * m should equal m
  AdjacencyMatrix result = ident * m;
  CHECK(result == m);
}

TEST_CASE(multiply_chain_of_three_nodes) {
  // 0->1, 1->2, 2->3  =>  m*m*m should have 0->3
  AdjacencyMatrix m{4};
  m.set(0, 1, true);
  m.set(1, 2, true);
  m.set(2, 3, true);
  AdjacencyMatrix m3 = (m * m) * m;
  CHECK(m3.at(0, 3) == true);
}

TEST_CASE(multiply_disconnected_graph) {
  // Two disconnected edges: 0->1 and 2->3
  AdjacencyMatrix m{4};
  m.set(0, 1, true);
  m.set(2, 3, true);
  AdjacencyMatrix mm = m * m;
  // No 2-step paths possible in this graph
  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 4; ++j)
      CHECK(mm.at(i, j) == false);
}

TEST_CASE(multiply_does_not_modify_operands) {
  AdjacencyMatrix a{3};
  a.set(0, 1, true);
  AdjacencyMatrix b{3};
  b.set(1, 2, true);
  AdjacencyMatrix orig_a{a};
  AdjacencyMatrix orig_b{b};
  (void)(a * b);
  CHECK(a == orig_a);
  CHECK(b == orig_b);
}

// ---------------------------------------------------------------------------
// Combined operations / regression
// ---------------------------------------------------------------------------

TEST_CASE(transpose_of_sum) {
  // (A + B)^T == A^T + B^T
  AdjacencyMatrix a{3};
  a.set(0, 1, true);
  AdjacencyMatrix b{3};
  b.set(2, 0, true);
  AdjacencyMatrix lhs = (a + b).transpose();
  AdjacencyMatrix rhs = a.transpose() + b.transpose();
  CHECK(lhs == rhs);
}

TEST_CASE(boundary_large_1x1_matrix) {
  AdjacencyMatrix m{1};
  m.set(0, 0, true);
  CHECK(m.at(0, 0) == true);
  AdjacencyMatrix t = m.transpose();
  CHECK(t.at(0, 0) == true);
  AdjacencyMatrix sum = m + m;
  CHECK(sum.at(0, 0) == true);
  AdjacencyMatrix prod = m * m;
  CHECK(prod.at(0, 0) == true);
}

int main() {
  return TestUtils::report_and_exit();
}