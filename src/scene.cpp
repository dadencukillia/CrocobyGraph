#include "scene.hpp"
#include "batch.hpp"
#include "components.hpp"
#include "entities.hpp"
#include "entt/entity/fwd.hpp"
#include <cstddef>
#include <variant>

namespace CrocobyGraph {

  void on_destroy_node(entt::registry& registry, entt::entity entity_to_destroy) {
    std::vector<entt::entity> to_remove;

    for (auto [entity, attachment] : registry.view<AttachComponent>().each()) {
      if (attachment.target == entity_to_destroy) to_remove.push_back(entity);
    }

    for (auto [entity, edge] : registry.view<EdgeEntity>().each()) {
      if (edge.node_start == entity_to_destroy || edge.node_end == entity_to_destroy) to_remove.push_back(entity);
    }

    for (auto entity : to_remove) registry.destroy(entity);
  }

  void on_destroy_edge(entt::registry& registry, entt::entity entity_to_destroy) {
    std::vector<entt::entity> to_remove;

    for (auto [entity, attachment] : registry.view<AttachComponent>().each()) {
      if (attachment.target == entity_to_destroy) to_remove.push_back(entity);
    }

    for (auto entity : to_remove) registry.destroy(entity);
  }

  Scene::Scene() {
    registry.on_destroy<NodeEntity>().connect<&on_destroy_node>();
    registry.on_destroy<EdgeEntity>().connect<&on_destroy_edge>();
  }

  void Scene::append(Batch&& batch, float offset_x, float offset_y) {
    std::vector<entt::entity> node_ids(batch.nodes_to_create.size());
    std::vector<entt::entity> edge_ids(batch.edges_to_create.size());
    std::vector<entt::entity> attach_label_ids(batch.attach_labels_to_create.size());
    std::vector<entt::entity> free_label_ids(batch.free_labels_to_create.size());

    for (size_t index = 0; index < batch.nodes_to_create.size(); ++index) {
      auto& node = batch.nodes_to_create[index];

      auto entity = registry.create();
      registry.emplace<NodeEntity>(entity, node.color, node.radius);
      registry.emplace<PositionComponent>(entity, node.position.x + offset_x, node.position.y + offset_y);

      node_ids[index] = entity;
    }

    for (size_t index = 0; index < batch.edges_to_create.size(); ++index) {
      auto& edge = batch.edges_to_create.at(index);

      entt::entity node_start;
      if (std::holds_alternative<BeingCreatedEntity>(edge.node_start)) {
        auto internal_id = std::get<BeingCreatedEntity>(edge.node_start);
        auto& id_pointer = batch.id_pointers[internal_id.id];
        assert(id_pointer.second == PrimitiveType::Node && "Not a node specified as the start of edge");

        node_start = node_ids[id_pointer.first];
      } else {
        node_start = std::get<entt::entity>(edge.node_start);
      }

      entt::entity node_end;
      if (std::holds_alternative<BeingCreatedEntity>(edge.node_end)) {
        auto internal_id = std::get<BeingCreatedEntity>(edge.node_end);
        auto& id_pointer = batch.id_pointers[internal_id.id];
        assert(id_pointer.second == PrimitiveType::Node && "Not a node specified as the end of edge");

        node_end = node_ids[id_pointer.first];
      } else {
        node_end = std::get<entt::entity>(edge.node_end);
      }

      auto entity = registry.create();
      registry.emplace<EdgeEntity>(entity, node_start, node_end, edge.arrow_on_start, edge.arrow_on_end, edge.color, edge.curve_type);

      edge_ids[index] = entity;
    }

    for (size_t index = 0; index < batch.free_labels_to_create.size(); ++index) {
      auto& label = batch.free_labels_to_create[index];

      auto entity = registry.create();
      registry.emplace<LabelEntity>(entity, label.first.label, label.first.color);
      registry.emplace<PositionComponent>(entity, label.second.x + offset_x, label.second.y + offset_y);

      free_label_ids[index] = entity;
    }

    for (size_t index = 0; index < batch.attach_labels_to_create.size(); ++index) {
      auto& label = batch.attach_labels_to_create.at(index);

      entt::entity attach_target;
      if (std::holds_alternative<BeingCreatedEntity>(label.second)) {
        auto internal_id = std::get<BeingCreatedEntity>(label.second);
        auto& id_pointer = batch.id_pointers[internal_id.id];
        assert((id_pointer.second == PrimitiveType::Node || id_pointer.second == PrimitiveType::Edge) && "Not a node or edge specified as an attach target");

        if (id_pointer.second == PrimitiveType::Node) {
          attach_target = node_ids[id_pointer.first];
        } else if (id_pointer.second == PrimitiveType::Edge) {
          attach_target = edge_ids[id_pointer.first];
        }
      } else {
        attach_target = std::get<entt::entity>(label.second);
      }

      auto entity = registry.create();
      registry.emplace<LabelEntity>(entity, label.first.label, label.first.color);
      registry.emplace<AttachComponent>(entity, attach_target, 0.0f, 0.0f);

      attach_label_ids[index] = entity;
    }
  }

  entt::registry& Scene::get_registry() {
    return registry;
  }

  void Scene::clear() {
    this->registry.clear();
  }

}
