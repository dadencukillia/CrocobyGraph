#include "adjacency_matrix.hpp"
#include <stdexcept>

namespace CrocobyGraph {

  AdjacencyMatrix::AdjacencyMatrix(size_t rows, size_t cols) :
    rows { rows },
    cols { cols },
    matrix { std::vector<uint8_t>(rows * cols, 0) } {}

  AdjacencyMatrix::AdjacencyMatrix(const AdjacencyMatrix& another) {
    rows = another.rows;
    cols = another.cols;
    matrix = another.matrix;
  }

  AdjacencyMatrix& AdjacencyMatrix::operator=(const AdjacencyMatrix& another) {
    rows = another.rows;
    cols = another.cols;
    matrix = another.matrix;
    return *this;
  }

  AdjacencyMatrix AdjacencyMatrix::transpose() const noexcept {
    AdjacencyMatrix new_matrix { cols, rows };
    for (size_t i = 0; i < rows; ++i) {
      for (size_t j = 0; j < cols; ++j) {
        new_matrix.matrix[j * rows + i] = matrix[i * cols + j];
      }
    }

    return new_matrix;
  }

  AdjacencyMatrix AdjacencyMatrix::operator+(const AdjacencyMatrix& another) {
    if (rows != another.rows || cols != another.cols) throw std::length_error("Matrices must have same size for this operation");

    AdjacencyMatrix new_matrix { rows, cols };
    for (size_t i = 0; i < rows * cols; ++i) {
      new_matrix.matrix[i] = matrix[i] || another.matrix[i];
    }

    return new_matrix;
  }

  AdjacencyMatrix AdjacencyMatrix::operator*(const AdjacencyMatrix& another) {
    if (cols != another.rows) throw std::length_error("Matrices must have corresponding sizes for this operation");

    AdjacencyMatrix new_matrix { rows, another.cols };
    const AdjacencyMatrix transposed { another.transpose() };

    for (size_t i = 0; i < new_matrix.rows; ++i) {
      for (size_t j = 0; j < new_matrix.cols; ++j) {
        bool r { false };
        for (size_t n = 0; n < cols && !r; ++n) {
          r = matrix[i * cols + n] && transposed.matrix[j * cols + n];
        }
        new_matrix.matrix[i * new_matrix.cols + j] = r;
      }
    }

    return new_matrix;
  }

}
