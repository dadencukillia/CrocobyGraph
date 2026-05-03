#ifndef _CGRAPH_INTERFACE_ADJACENCY_MATRIX_HPP_
#define _CGRAPH_INTERFACE_ADJACENCY_MATRIX_HPP_

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace CrocobyGraph {

  class AdjacencyMatrix {
    std::vector<uint8_t> matrix;
    size_t rows { 0 };
    size_t cols { 0 };

  public:
    AdjacencyMatrix() = delete;

    AdjacencyMatrix(size_t rows, size_t cols);
    AdjacencyMatrix(const AdjacencyMatrix& another);
    ~AdjacencyMatrix() noexcept = default;
    AdjacencyMatrix& operator=(const AdjacencyMatrix& another);

    [[nodiscard]] inline bool at(size_t row, size_t col) const {
      assert(col < cols && row < rows && "Out of size");
      return matrix[row * cols + col];
    }

    void inline set(size_t row, size_t col, bool value) {
      assert(col < cols && row < rows && "Out of size");
      matrix[row * cols + col] = value;
    }

    [[nodiscard]] AdjacencyMatrix transpose() const noexcept;
    [[nodiscard]] AdjacencyMatrix operator+(const AdjacencyMatrix& another);
    [[nodiscard]] AdjacencyMatrix operator*(const AdjacencyMatrix& another);
  };

}

#endif
