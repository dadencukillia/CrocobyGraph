#include "batch.hpp"
#include <utility>

namespace CrocobyGraph {

  BeingCreatedEntity Batch::add_node(NodeBundle&& node) {
    size_t node_index = nodes_to_create.size();
    size_t entity_index = id_map_primitive.size();
    nodes_to_create.push_back(std::move(node));
    id_map_primitive.push_back(PrimitiveType::Node);

    return { entity_index };
  }

  BeingCreatedEntity Batch::add_node(NodeBundle&& node, LabelEntity&& label) {
    auto id = add_node(std::move(node));
    add_label(std::move(label), id);
    return id;
  }

  BeingCreatedEntity Batch::add_edge(EdgeBundle&& edge) {
    size_t edge_index = edges_to_create.size();
    size_t entity_index = id_map_primitive.size();
    edges_to_create.push_back(std::move(edge));
    id_map_primitive.push_back(PrimitiveType::Edge);

    return { entity_index };
  }

  BeingCreatedEntity Batch::add_edge(EdgeBundle&& edge, LabelEntity&& label) {
    auto id = add_edge(std::move(edge));
    add_label(std::move(label), id);
    return id;
  }

  BeingCreatedEntity Batch::add_label(LabelEntity&& label, PositionComponent&& position) {
    size_t free_label_index = free_labels_to_create.size();
    size_t entity_index = id_map_primitive.size();
    free_labels_to_create.push_back(std::make_pair(std::move(label), std::move(position)));
    id_map_primitive.push_back(PrimitiveType::FreeLabel);

    return { entity_index };
  }

  BeingCreatedEntity Batch::add_label(LabelEntity&& label, Entity&& entity) {
    size_t attach_label_index = attach_labels_to_create.size();
    size_t entity_index = id_map_primitive.size();
    attach_labels_to_create.push_back(std::make_pair(std::move(label), std::move(entity)));
    id_map_primitive.push_back(PrimitiveType::AttachLabel);

    return { entity_index };
  }

}
