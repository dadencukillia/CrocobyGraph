#include "adjacency_matrix.hpp"
#include <cstddef>
#include <stdexcept>

namespace CrocobyGraph {

  AdjacencyMatrix::AdjacencyMatrix(size_t nodes) :
    nodes { nodes },
    matrix { std::vector<uint8_t>(nodes * nodes, 0) } {}

  AdjacencyMatrix::AdjacencyMatrix(const AdjacencyMatrix& another) {
    nodes = another.nodes;
    matrix = another.matrix;
  }

  AdjacencyMatrix& AdjacencyMatrix::operator=(const AdjacencyMatrix& another) {
    nodes = another.nodes;
    matrix = another.matrix;
    return *this;
  }

  AdjacencyMatrix AdjacencyMatrix::transpose() const noexcept {
    AdjacencyMatrix new_matrix { nodes };
    for (size_t i = 0; i < nodes; ++i) {
      for (size_t j = 0; j < nodes; ++j) {
        new_matrix.matrix[j * nodes + i] = matrix[i * nodes + j];
      }
    }

    return new_matrix;
  }

  AdjacencyMatrix AdjacencyMatrix::operator+(const AdjacencyMatrix& another) {
    if (nodes != another.nodes) throw std::length_error("Matrices must have same size for this operation");

    AdjacencyMatrix new_matrix { nodes };
    for (size_t i = 0; i < nodes * nodes; ++i) {
      new_matrix.matrix[i] = matrix[i] || another.matrix[i];
    }

    return new_matrix;
  }

  AdjacencyMatrix AdjacencyMatrix::operator*(const AdjacencyMatrix& another) {
    if (nodes != another.nodes) throw std::length_error("Matrices must have same size for this operation");

    AdjacencyMatrix new_matrix { nodes };
    const AdjacencyMatrix transposed { another.transpose() };

    for (size_t i = 0; i < nodes; ++i) {
      for (size_t j = 0; j < nodes; ++j) {
        bool r { false };
        for (size_t n = 0; n < nodes && !r; ++n) {
          r = matrix[i * nodes + n] && transposed.matrix[j * nodes + n];
        }
        new_matrix.matrix[i * nodes + j] = r;
      }
    }

    return new_matrix;
  }

}
