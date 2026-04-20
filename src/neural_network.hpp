#ifndef _CGRAPH_NEURAL_NETWORK_HPP_
#define _CGRAPH_NEURAL_NETWORK_HPP_

#include "batch.hpp"
#include "config.hpp"
#include "entities.hpp"
#include <cassert>
#include <string>

namespace CrocobyGraph {

  template <unsigned int IN_COUNT, unsigned int HIDDEN_LAYERS_COUNT, unsigned int OUT_COUNT>
  inline Batch neural_network(
    const std::string (&&in_names)[IN_COUNT],
    const unsigned int (&hidden_neurons)[HIDDEN_LAYERS_COUNT],
    const std::string (&&out_names)[OUT_COUNT],
    float spacing_x = 7.0f * DEFAULT_NODE_RADIUS,
    float spacing_y = 3.5f * DEFAULT_NODE_RADIUS
  ) {
    static_assert(IN_COUNT != 0);
    static_assert(OUT_COUNT != 0);

    unsigned int width = HIDDEN_LAYERS_COUNT + 1;

    Batch batch {};
    std::vector<BeingCreatedEntity> prev_layer_neurons;

    for (unsigned int in = 0; in < IN_COUNT; ++in) {
      auto id = batch.add_node(NodeBundle {
        .color = { 0x00FF00FF },
        .position = { 
          (spacing_x * width) / -2.0f,
          (static_cast<float>(in) - static_cast<float>(IN_COUNT - 1) * 0.5f) * spacing_y
        }
      }, LabelEntity {
        .label = std::move(in_names[in]),
        .color = { 0x000000FF }
      });
      prev_layer_neurons.push_back(id);
    }

    for (unsigned int hidden_layer = 0; hidden_layer < HIDDEN_LAYERS_COUNT; ++hidden_layer) {
      const auto& neuron_counts = hidden_neurons[hidden_layer];
      assert(neuron_counts != 0);

      std::vector<BeingCreatedEntity> current_layer_neurons(neuron_counts);
      for (unsigned int neuron = 0; neuron < neuron_counts; ++neuron) {
        current_layer_neurons[neuron] = batch.add_node(NodeBundle {
          .color = { 0xFFFFFFFF },
          .position = {
            (static_cast<float>(hidden_layer + 1) - static_cast<float>(width) * 0.5f) * spacing_x,
            (static_cast<float>(neuron) - static_cast<float>(neuron_counts - 1) * 0.5f) * spacing_y,
          }
        });

        for (auto& prev_neuron : prev_layer_neurons) {
          batch.add_edge(EdgeBundle {
            .node_start = prev_neuron,
            .node_end = current_layer_neurons[neuron],
            .arrow_on_start = false,
            .arrow_on_end = true,
            .color = { 0xFFFFFFFF },
          });
        }
      }

      prev_layer_neurons = std::move(current_layer_neurons);
    }

    for (unsigned int in = 0; in < OUT_COUNT; ++in) {
      auto id = batch.add_node(NodeBundle {
        .color = { 0xFF0000FF },
        .position = { 
          (spacing_x * width) / 2.0f,
          (static_cast<float>(in) - static_cast<float>(OUT_COUNT - 1) * 0.5f) * spacing_y
        }
      }, LabelEntity {
        .label = std::move(out_names[in]),
        .color = { 0x000000FF }
      });

      for (auto& prev_neuron : prev_layer_neurons) {
        batch.add_edge(EdgeBundle {
          .node_start = prev_neuron,
          .node_end = id,
          .arrow_on_start = false,
          .arrow_on_end = true,
          .color = { 0xFFFFFFFF },
        });
      }
    }

    return batch;
  }

}

#endif
