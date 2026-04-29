#include "batch.hpp"
#include <utility>

namespace CrocobyGraph {

  const EasyNode& EasyNode::add_label(LabelEntity&& label, BeingCreatedEntity* label_ptr) const noexcept {
    auto label_id = batch.add_label(std::move(label), id);
    if (label_ptr) *label_ptr = label_id;
    return *this;
  }

  const EasyNode& EasyNode::connect(const EasyNode& another, BeingCreatedEntity* edge_ptr, Color color, bool arrow_on_start, bool arrow_on_end, EdgeCurveType curve) const noexcept {
    auto edge_id = batch.add_edge(EdgeBundle {
      .node_start = id,
      .node_end = another.id,
      .arrow_on_start = arrow_on_start,
      .arrow_on_end = arrow_on_end,
      .color = color,
      .curve_type = curve
    });
    if (edge_ptr) *edge_ptr = edge_id;
    return *this;
  }

  const EasyNode& EasyNode::set_color(Color new_color) const noexcept {
    node_bundle.color = new_color;
    return *this;
  }

  const EasyNode& EasyNode::set_radius(float new_radius) const noexcept {
    node_bundle.radius = new_radius;
    return *this;
  }

  const EasyNode& EasyNode::set_pos(Vector pos) const noexcept {
    node_bundle.position = { pos.x, pos.y };
    return *this;
  }

  const EasyNode& EasyNode::operator|(Color new_color) const noexcept {
    return set_color(new_color);
  }

  const EasyNode& EasyNode::operator|(float new_radius) const noexcept {
    return set_radius(new_radius);
  }

  const EasyNode& EasyNode::operator|(Vector pos) const noexcept {
    return set_pos(pos);
  }

  const EasyNode& EasyNode::operator|(std::string&& text) const noexcept {
    return add_label(LabelEntity {
      .label = text
    });
  }

  const EasyNode& EasyNode::operator==(const EasyNode& another) const noexcept {
    connect(another);
    return another;
  }

  const EasyNode& EasyNode::operator>>(const EasyNode& another) const noexcept {
    connect(another, nullptr, DEFAULT_EDGE_COLOR, false, true);
    return another;
  }

  const EasyNode& EasyNode::operator<<(const EasyNode& another) const noexcept {
    connect(another, nullptr, DEFAULT_EDGE_COLOR, true, false);
    return another;
  }

  const EasyNode& EasyNode::operator^(const EasyNode& another) const noexcept {
    connect(another, nullptr, DEFAULT_EDGE_COLOR, true, true);
    return another;
  }

  entt::entity EasyNode::operator[](const std::vector<entt::entity> ecs_ids) const {
    return ecs_ids[id.id];
  }

  EasyNode::operator BeingCreatedEntity() const noexcept {
    return id;
  }

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

  BeingCreatedEntity Batch::add_label(LabelEntity&& label, Entity entity) {
    size_t attach_label_index = attach_labels_to_create.size();
    size_t entity_index = id_map_primitive.size();
    attach_labels_to_create.push_back(std::make_pair(std::move(label), entity));
    id_map_primitive.push_back(PrimitiveType::AttachLabel);

    return { entity_index };
  }

  EasyNode Batch::easy_node() {
    size_t node_index = nodes_to_create.size();
    auto id = add_node(NodeBundle {});
    return EasyNode(*this, nodes_to_create[node_index], id);
  }

}
