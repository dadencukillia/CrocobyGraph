#ifndef _CGRAPH_INTERFACE_BATCH_HPP_
#define _CGRAPH_INTERFACE_BATCH_HPP_

#include "../config.hpp"
#include "color.hpp"
#include "entities.hpp"
#include "components.hpp"
#include "entt/entity/fwd.hpp"
#include "vector.hpp"
#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>

namespace CrocobyGraph {

  struct BeingCreatedEntity {
    size_t id;
  };

  using Entity = std::variant<BeingCreatedEntity, entt::entity>;

  struct NodeBundle {
    Color color { DEFAULT_NODE_COLOR };
    float radius { DEFAULT_NODE_RADIUS };
    PositionComponent position;
  };

  struct EdgeBundle {
    Entity node_start { entt::entity { 0xFFFFFFFF } };
    Entity node_end { entt::entity { 0xFFFFFFFF } };
    bool arrow_on_start { false };
    bool arrow_on_end { false };
    Color color { DEFAULT_EDGE_COLOR };
    EdgeCurveType curve_type { EdgeCurveType::Linear };
  };

  enum class PrimitiveType : uint8_t {
    Node,
    Edge,
    AttachLabel,
    FreeLabel
  };

  class Batch;

  class EasyNode {
    Batch& batch;
    NodeBundle& node_bundle;
    BeingCreatedEntity id;

    friend class Batch;

    EasyNode(Batch& batch, NodeBundle& node_bundle, BeingCreatedEntity id) :
      batch { batch },
      node_bundle { node_bundle },
      id { id } {}

  public:
    EasyNode() = delete;
    EasyNode(const EasyNode&) = default;
    EasyNode& operator=(const EasyNode&) = delete;
    EasyNode(EasyNode&& other) noexcept = delete;
    EasyNode& operator=(EasyNode&& other) noexcept = delete;

    const EasyNode& add_label(LabelEntity&&, BeingCreatedEntity* label_ptr = nullptr) const noexcept;
    const EasyNode& connect(const EasyNode&, BeingCreatedEntity* edge_ptr = nullptr, Color color = DEFAULT_EDGE_COLOR, bool arrow_on_start = false, bool arrow_on_end = false, EdgeCurveType curve = EdgeCurveType::Linear) const noexcept;
    const EasyNode& set_color(Color new_color) const noexcept;
    const EasyNode& set_radius(float new_radius) const noexcept;
    const EasyNode& set_pos(Vector pos) const noexcept;

    const EasyNode& operator|(Color new_color) const noexcept;
    const EasyNode& operator|(float new_radius) const noexcept;
    const EasyNode& operator|(Vector pos) const noexcept;
    const EasyNode& operator|(std::string&& text) const noexcept;
    const EasyNode& operator==(const EasyNode& another) const noexcept;
    const EasyNode& operator>>(const EasyNode& another) const noexcept;
    const EasyNode& operator<<(const EasyNode& another) const noexcept;
    const EasyNode& operator^(const EasyNode& another) const noexcept;

    entt::entity operator[](const std::vector<entt::entity> ecs_ids) const;
    operator BeingCreatedEntity() const noexcept;
  };

  class Batch {
    std::vector<NodeBundle> nodes_to_create;
    std::vector<EdgeBundle> edges_to_create;
    std::vector<std::pair<LabelEntity, Entity>> attach_labels_to_create;
    std::vector<std::pair<LabelEntity, PositionComponent>> free_labels_to_create;
    std::vector<PrimitiveType> id_map_primitive;

  public:
    BeingCreatedEntity add_node(NodeBundle&&);
    BeingCreatedEntity add_node(NodeBundle&&, LabelEntity&&);
    BeingCreatedEntity add_edge(EdgeBundle&&);
    BeingCreatedEntity add_edge(EdgeBundle&&, LabelEntity&&);
    BeingCreatedEntity add_label(LabelEntity&&, PositionComponent&&);
    BeingCreatedEntity add_label(LabelEntity&&, Entity);
    EasyNode easy_node();

    Batch() = default;
    Batch(Batch&&) noexcept = default;
    Batch& operator=(Batch&&) noexcept = default;
    Batch(const Batch&) = delete;

    friend class Scene;
  };

}

#endif 
