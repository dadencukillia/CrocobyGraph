#include "scene.hpp"
#include "batch.hpp"
#include "components.hpp"
#include "entities.hpp"
#include "entt/entt.hpp"
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
    registry = new entt::registry();
    registry->on_destroy<NodeEntity>().connect<&on_destroy_node>();
    registry->on_destroy<EdgeEntity>().connect<&on_destroy_edge>();
  }

  Scene::~Scene() {
    delete registry;
  }

  std::vector<entt::entity> Scene::append(Batch&& batch, float offset_x, float offset_y) {
    size_t created_nodes { 0 };
    size_t created_edges { 0 };
    size_t created_attach_labels { 0 };
    size_t created_free_labels { 0 };
    std::vector<entt::entity> ecs_ids(batch.id_map_primitive.size());

    registry->storage<entt::entity>().reserve(batch.id_map_primitive.size());

    registry->storage<NodeEntity>().reserve(batch.nodes_to_create.size());
    registry->storage<EdgeEntity>().reserve(batch.edges_to_create.size());
    registry->storage<LabelEntity>().reserve(batch.free_labels_to_create.size() + batch.attach_labels_to_create.size());

    registry->storage<PositionComponent>().reserve(batch.nodes_to_create.size() + batch.free_labels_to_create.size());

    for (size_t internal_id = 0; internal_id < batch.id_map_primitive.size(); internal_id++) {
      auto primitive = batch.id_map_primitive[internal_id];

      switch (primitive) {
        case PrimitiveType::Node: {
          auto& node = batch.nodes_to_create[created_nodes++];

          auto node_entity = registry->create();
          registry->emplace<NodeEntity>(node_entity, node.color, node.radius);
          registry->emplace<PositionComponent>(node_entity, node.position.x + offset_x, node.position.y + offset_y);
          ecs_ids[internal_id] = node_entity;

          break;
        }

        case PrimitiveType::Edge: {
          auto& edge = batch.edges_to_create[created_edges++];

          entt::entity node_start;
        
          if (auto* node_start_internal_id = std::get_if<BeingCreatedEntity>(&edge.node_start)) {
            auto node_start_primitive = batch.id_map_primitive[node_start_internal_id->id];
            assert(node_start_primitive == PrimitiveType::Node && "Not a node specified as the start of edge");

            node_start = ecs_ids[node_start_internal_id->id];
          } else {
            node_start = std::get<entt::entity>(edge.node_start);
          }

          entt::entity node_end;
          if (auto* node_end_internal_id = std::get_if<BeingCreatedEntity>(&edge.node_end)) {
            auto node_end_primitive = batch.id_map_primitive[node_end_internal_id->id];
            assert(node_end_primitive == PrimitiveType::Node && "Not a node specified as the end of edge");

            node_end = ecs_ids[node_end_internal_id->id];
          } else {
            node_end = std::get<entt::entity>(edge.node_end);
          }

          auto edge_entity = registry->create();
          registry->emplace<EdgeEntity>(edge_entity, node_start, node_end, edge.arrow_on_start, edge.arrow_on_end, edge.color, edge.curve_type);
          ecs_ids[internal_id] = edge_entity;

          break;
        }

        case PrimitiveType::FreeLabel: {
          auto& free_label = batch.free_labels_to_create[created_free_labels++];

          auto free_label_entity = registry->create();
          registry->emplace<LabelEntity>(free_label_entity, std::move(free_label.first.label), free_label.first.color);
          registry->emplace<PositionComponent>(free_label_entity, free_label.second.x + offset_x, free_label.second.y + offset_y);
          ecs_ids[internal_id] = free_label_entity;

          break;
        }

        case PrimitiveType::AttachLabel: {
          auto& attach_label = batch.attach_labels_to_create[created_attach_labels++];

          entt::entity attach_target;
          if (auto* target_internal_id = std::get_if<BeingCreatedEntity>(&attach_label.second)) {
            auto target_primitive = batch.id_map_primitive[target_internal_id->id];
            assert((target_primitive == PrimitiveType::Node || target_primitive == PrimitiveType::Edge) && "Not a node or edge specified as an attach target");

            attach_target = ecs_ids[target_internal_id->id];
          } else {
            attach_target = std::get<entt::entity>(attach_label.second);
          }

          auto attach_label_entity = registry->create();
          registry->emplace<LabelEntity>(attach_label_entity, std::move(attach_label.first.label), attach_label.first.color);
          registry->emplace<AttachComponent>(attach_label_entity, attach_target, 0.0f, 0.0f);
          ecs_ids[internal_id] = attach_label_entity;

          break;
        }
      }
    }

    return ecs_ids;
  }

  entt::registry& Scene::get_registry() {
    return *registry;
  }

  void Scene::clear() {
    registry->clear();
  }

}
