#include "scene.hpp"
#include "../internal/labels.hpp"
#include "batch.hpp"
#include "components.hpp"
#include "entities.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include <cassert>
#include <cstddef>
#include <stdexcept>
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

  entt::registry& Scene::get_registry() {
    return *registry;
  }

  void Scene::clear() {
    registry->clear();
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

  Scene& Scene::operator+=(Batch&& batch) {
    append(std::move(batch));
    return *this;
  }

  PrimitiveType Scene::type(entt::entity entity) const {
    if (is_node(entity)) return PrimitiveType::Node;
    else if (is_edge(entity)) return PrimitiveType::Edge;
    else if (is_free_label(entity)) return PrimitiveType::FreeLabel;
    else if (is_attach_label(entity)) return PrimitiveType::AttachLabel;
    else throw std::logic_error("Unknown type of the specified entity");
  }

  Vector Scene::pos(entt::entity entity) const {
    if (registry->any_of<PositionComponent>(entity)) {
      const auto pos_component = registry->get<const PositionComponent>(entity);
      return { pos_component.x, pos_component.y };
    } else if (is_attach_label(entity)) {
      const auto label_pos = get_attach_label_position(*registry, entity);
      return { label_pos.x, label_pos.y };
    } else if (is_edge(entity)) {
      const auto& edge = registry->get<const EdgeEntity>(entity);
      const auto pos_component = registry->get<const PositionComponent>(edge.node_start);
      return { pos_component.x, pos_component.y };
    } else {
      throw std::logic_error("Unknown type of the specified entity");
    }
  }

  Vector Scene::pos_second(entt::entity entity) const {
    if (is_edge(entity)) {
      const auto& edge = registry->get<const EdgeEntity>(entity);
      const auto pos_component = registry->get<const PositionComponent>(edge.node_end);
      return { pos_component.x, pos_component.y };
    } else return pos(entity);
  }

  void Scene::set_pos(entt::entity entity, Vector pos) {
    if (registry->any_of<PositionComponent>(entity)) {
      auto& pos_component = registry->get<PositionComponent>(entity);
      pos_component.x = pos.x;
      pos_component.y = pos.y;
    } else if (is_attach_label(entity)) {
      auto& attach = registry->get<AttachComponent>(entity);
      const auto label_pos = get_attach_label_position(*registry, entity);
      attach.offset_x += pos.x - label_pos.x;
      attach.offset_y += pos.y - label_pos.y;
    } else {
      throw std::logic_error("Unknown type of the specified entity");
    }
  }

  Color Scene::color(entt::entity entity) const {
    if (is_node(entity)) {
      return registry->get<const NodeEntity>(entity).color;
    } else if (is_edge(entity)) {
      return registry->get<const EdgeEntity>(entity).color;
    } else if (is_any_label(entity)) {
      return registry->get<const LabelEntity>(entity).color;
    } else {
      throw std::logic_error("Unknown type of the specified entity");
    }
  }

  void Scene::set_color(entt::entity entity, Color color) {
    if (is_node(entity)) {
      registry->get<NodeEntity>(entity).color = color;
    } else if (is_edge(entity)) {
      registry->get<EdgeEntity>(entity).color = color;
    } else if (is_any_label(entity)) {
      registry->get<LabelEntity>(entity).color = color;
    } else {
      throw std::logic_error("Unknown type of the specified entity");
    }
  }

  std::string_view Scene::label_text(entt::entity entity) const {
    assert(is_any_label(entity) && "The specified entity must be a label");
    return registry->get<const LabelEntity>(entity).label;
  }

  void Scene::set_label_text(entt::entity entity, std::string&& text) {
    assert(is_any_label(entity) && "The specified entity must be a label");
    registry->get<LabelEntity>(entity).label = std::move(text);
  }

  Vector Scene::label_offset(entt::entity entity) const {
    assert(is_attach_label(entity) && "The specified entity must be an attach label");
    const auto attach = registry->get<const AttachComponent>(entity);
    return { attach.offset_x, attach.offset_y };
  }

  void Scene::set_label_offset(entt::entity entity, Vector offset) {
    assert(is_attach_label(entity) && "The specified entity must be an attach label");
    auto& attach = registry->get<AttachComponent>(entity);
    attach.offset_x = offset.x;
    attach.offset_y = offset.y;
  }

  std::vector<entt::entity> Scene::nodes() const {
    std::vector<entt::entity> result;
    for (const auto& [entity, node] : registry->view<const NodeEntity>().each()) {
      result.push_back(entity);
    }

    return result;
  }

  std::vector<entt::entity> Scene::node_neighbors(entt::entity entity) const {
    assert(is_node(entity) && "The specified entity must be a node");
    std::vector<entt::entity> result;
    for (const auto& [edge_entity, edge] : registry->view<const EdgeEntity>().each()) {
      if (edge.node_start == entity) {
        result.push_back(edge.node_end);
      } else if (edge.node_end == entity) {
        result.push_back(edge.node_start);
      }
    }

    return result;
  }

  std::vector<entt::entity> Scene::node_edges(entt::entity entity) const {
    std::vector<entt::entity> result;
    for (const auto& [edge_entity, edge] : registry->view<const EdgeEntity>().each()) {
      result.push_back(edge_entity);
    }

    return result;
  }

  std::vector<entt::entity> Scene::node_labels(entt::entity entity) const {
    std::vector<entt::entity> result;
    for (const auto& [label_entity, label, attach] : registry->view<const LabelEntity, const AttachComponent>().each()) {
      if (attach.target == entity) result.push_back(label_entity);
    }

    return result;
  }

  entt::entity Scene::node_set_label(entt::entity entity, std::string&& text, Color color) {
    std::vector<entt::entity> remove_list;
    entt::entity found { entt::null };

    for (const auto& [label_entity, label, attach] : registry->view<LabelEntity, const AttachComponent>().each()) {
      if (attach.target == entity) {
        if (found == entt::null) {
          label.label = std::move(text);
          label.color = color;
          found = label_entity;
        } else {
          remove_list.push_back(label_entity);
        }
      }
    }

    if (found == entt::null) {
      return node_add_label(entity, std::move(text));
    } else {
      for (auto entity : remove_list) {
        registry->destroy(entity);
      }
      return found;
    }
  }

  bool Scene::nodes_connected(entt::entity node_a, entt::entity node_b) const {
    for (const auto& [entity, edge] : registry->view<const EdgeEntity>().each()) {
      if (edge.node_start == node_a && edge.node_end == node_b || edge.node_start == node_b && edge.node_end == node_a) return true;
    }

    return false;
  }

  entt::entity Scene::nodes_connect(entt::entity node_a, entt::entity node_b) {
    assert(!nodes_connected(node_a, node_b) && "Nodes must be disconnected for this operation");
    auto edge = registry->create();
    registry->emplace<EdgeEntity>(edge, EdgeEntity {
      .node_start = node_a,
      .node_end = node_b
    });

    return edge;
  }

  void Scene::nodes_disconnect(entt::entity node_a, entt::entity node_b) {
    for (const auto& [edge_entity, edge] : registry->view<const EdgeEntity>().each()) {
      if ((edge.node_start == node_a && edge.node_end == node_b) || (edge.node_start == node_b && edge.node_end == node_a)) {
        registry->destroy(edge_entity);
        return;
      }
    }
  }

  entt::entity Scene::node_add_label(entt::entity entity, std::string&& text, Color color) {
    auto label = registry->create();
    registry->emplace<LabelEntity>(label, LabelEntity {
      .label = std::move(text),
      .color = color
    });
    registry->emplace<AttachComponent>(label, entity, 0.0f, 0.0f);

    return label;
  }

  float Scene::node_radius(entt::entity entity) const {
    assert(is_node(entity) && "The specified entity must be a node");
    return registry->get<const NodeEntity>(entity).radius;
  }

  void Scene::node_radius(entt::entity entity, float radius) {
    assert(is_node(entity) && "The specified entity must be a node");
    registry->get<NodeEntity>(entity).radius = radius;
  }

  std::pair<entt::entity, entt::entity> Scene::edge_nodes(entt::entity entity) const {
    assert(is_edge(entity) && "The specified entity must be an edge");
    const auto& edge = registry->get<const EdgeEntity>(entity);
    return { edge.node_start, edge.node_end };
  }

  std::vector<entt::entity> Scene::edge_labels(entt::entity entity) const {
    return node_labels(entity);
  }

  entt::entity Scene::edge_set_label(entt::entity entity, std::string&& text, Color color) {
    return node_set_label(entity, std::move(text), color);
  }

  std::pair<bool, bool> Scene::edge_arrows(entt::entity entity) const {
    assert(is_edge(entity) && "The specified entity must be an edge");
    const auto& edge = registry->get<const EdgeEntity>(entity);
    return { edge.arrow_on_start, edge.arrow_on_end };
  }

  void Scene::set_edge_arrows(entt::entity entity, std::pair<bool, bool> arrows) {
    assert(is_edge(entity) && "The specified entity must be an edge");
    auto& edge = registry->get<EdgeEntity>(entity);
    edge.arrow_on_start = arrows.first;
    edge.arrow_on_end = arrows.second;
  }

  EdgeCurveType Scene::edge_curve(entt::entity entity) const {
    assert(is_edge(entity) && "The specified entity must be an edge");
    return registry->get<const EdgeEntity>(entity).curve_type;
  }

  void Scene::set_edge_curve(entt::entity entity, EdgeCurveType curve_type) {
    assert(is_edge(entity) && "The specified entity must be an edge");
    registry->get<EdgeEntity>(entity).curve_type = curve_type;
  }

  entt::entity Scene::edge_add_label(entt::entity entity, std::string&& text, Color color) {
    return node_add_label(entity, std::move(text));
  }

  entt::entity Scene::edge_between(entt::entity node_a, entt::entity node_b) const {
    for (const auto& [entity, edge] : registry->view<const EdgeEntity>().each()) {
      if ((edge.node_start == node_a && edge.node_end == node_b) || (edge.node_start == node_b && edge.node_end == node_a)) {
        return entity;
      }
    }

    return entt::null;
  }

  bool Scene::is_self_loop(entt::entity entity) const {
    assert(is_edge(entity) && "The specified entity must be an edge");
    const auto& edge = registry->get<const EdgeEntity>(entity);
    return edge.node_start == edge.node_end;
  }

  bool Scene::edge_points_to(entt::entity edge, entt::entity node) const {
    assert(is_edge(edge) && "The first specified entity must be an edge");
    assert(is_node(node) && "The second specified entity must be a node");
    const auto& edge_entity = registry->get<const EdgeEntity>(edge);
    return edge_entity.arrow_on_start && edge_entity.node_start == node || edge_entity.arrow_on_end && edge_entity.node_end == node;
  }

  bool Scene::is_directed_edge(entt::entity entity) const {
    assert(is_edge(entity) && "The specified entity must be an edge");
    const auto& edge = registry->get<const EdgeEntity>(entity);
    return edge.arrow_on_start || edge.arrow_on_end;
  }

  bool Scene::is_node(entt::entity entity) const {
    return registry->any_of<NodeEntity>(entity);
  }

  bool Scene::is_edge(entt::entity entity) const {
    return registry->any_of<EdgeEntity>(entity);
  }

  bool Scene::is_any_label(entt::entity entity) const {
    return registry->any_of<LabelEntity>(entity);
  }

  bool Scene::is_free_label(entt::entity entity) const {
    return registry->all_of<LabelEntity, PositionComponent>(entity);
  }

  bool Scene::is_attach_label(entt::entity entity) const {
    return registry->all_of<LabelEntity, AttachComponent>(entity);
  }

}
