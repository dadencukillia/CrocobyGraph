#ifndef _CGRAPH_INTERFACE_ADJACENCY_MATRIX_HPP_
#define _CGRAPH_INTERFACE_ADJACENCY_MATRIX_HPP_

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace CrocobyGraph {

  class AdjacencyMatrix {
    std::vector<uint8_t> matrix;
    size_t nodes { 0 };

  public:
    AdjacencyMatrix() = delete;

    AdjacencyMatrix(size_t nodes);
    AdjacencyMatrix(const AdjacencyMatrix& another);
    ~AdjacencyMatrix() noexcept = default;
    AdjacencyMatrix& operator=(const AdjacencyMatrix& another);

    [[nodiscard]] inline size_t size() const noexcept {
      return nodes;
    }

    [[nodiscard]] inline bool at(size_t node, size_t connection_node) const noexcept {
      assert(node < nodes && connection_node < nodes && "Out of size");
      return matrix[node * nodes + connection_node];
    }

    void inline set(size_t node, size_t connection_node, bool value) noexcept {
      assert(node < nodes && connection_node < nodes && "Out of size");
      matrix[node * nodes + connection_node] = value;
    }

    [[nodiscard]] inline bool operator==(const AdjacencyMatrix& another) const noexcept {
      return matrix == another.matrix;
    }

    [[nodiscard]] AdjacencyMatrix transpose() const noexcept;
    [[nodiscard]] AdjacencyMatrix operator+(const AdjacencyMatrix& another);
    [[nodiscard]] AdjacencyMatrix operator*(const AdjacencyMatrix& another);
  };

}

#endif
