#include "batch.hpp"
#include <utility>

namespace CrocobyGraph {

  BeingCreatedEntity Batch::add_node(NodeBundle&& node) {
    size_t node_index = nodes_to_create.size();
    size_t entity_index = id_pointers.size();
    nodes_to_create.push_back(std::move(node));
    id_pointers.push_back(std::make_pair(node_index, PrimitiveType::Node));

    return { entity_index };
  }

  BeingCreatedEntity Batch::add_node(NodeBundle&& node, LabelEntity&& label) {
    auto id = add_node(std::move(node));
    add_label(std::move(label), id);
    return id;
  }

  BeingCreatedEntity Batch::add_edge(EdgeBundle&& edge) {
    size_t edge_index = edges_to_create.size();
    size_t entity_index = id_pointers.size();
    edges_to_create.push_back(std::move(edge));
    id_pointers.push_back(std::make_pair(edge_index, PrimitiveType::Edge));

    return { entity_index };
  }

  BeingCreatedEntity Batch::add_edge(EdgeBundle&& edge, LabelEntity&& label) {
    auto id = add_edge(std::move(edge));
    add_label(std::move(label), id);
    return id;
  }

  BeingCreatedEntity Batch::add_label(LabelEntity&& label, PositionComponent&& position) {
    size_t free_label_index = free_labels_to_create.size();
    size_t entity_index = id_pointers.size();
    free_labels_to_create.push_back(std::make_pair(std::move(label), std::move(position)));
    id_pointers.push_back(std::make_pair(free_label_index, PrimitiveType::FreeLabel));

    return { entity_index };
  }

  BeingCreatedEntity Batch::add_label(LabelEntity&& label, Entity&& entity) {
    size_t attach_label_index = attach_labels_to_create.size();
    size_t entity_index = id_pointers.size();
    attach_labels_to_create.push_back(std::make_pair(std::move(label), std::move(entity)));
    id_pointers.push_back(std::make_pair(attach_label_index, PrimitiveType::AttachLabel));

    return { entity_index };
  }

}
