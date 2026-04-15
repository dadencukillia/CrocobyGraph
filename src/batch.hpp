#ifndef _CGRAPH_BATCH_HPP_
#define _CGRAPH_BATCH_HPP_

#include "entities.hpp"
#include "components.hpp"
#include <cstddef>
#include <variant>
#include <vector>

namespace CrocobyGraph {

  struct BeingCreatedEntity {
    size_t id;
  };

  using Entity = std::variant<BeingCreatedEntity, entt::entity>;

  struct NodeBundle {
    Color color { ColorPresets::WHITE };
    double radius { 20.0 };
    PositionComponent position;
  };

  struct EdgeBundle {
    Entity node_start { entt::null };
    Entity node_end { entt::null };
    bool arrow_on_start { false };
    bool arrow_on_end { false };
    Color color { ColorPresets::WHITE };
    EdgeCurveType curve_type { EdgeCurveType::Linear };
  };

  enum class PrimitiveType {
    Node,
    Edge,
    AttachLabel,
    FreeLabel
  };

  class Batch {
    std::vector<NodeBundle> nodes_to_create;
    std::vector<EdgeBundle> edges_to_create;
    std::vector<std::pair<LabelEntity, Entity>> attach_labels_to_create;
    std::vector<std::pair<LabelEntity, PositionComponent>> free_labels_to_create;

    std::vector<std::pair<size_t, PrimitiveType>> id_pointers;

  public:
    BeingCreatedEntity add_node(NodeBundle&&);
    BeingCreatedEntity add_node(NodeBundle&&, LabelEntity&&);
    BeingCreatedEntity add_edge(EdgeBundle&&);
    BeingCreatedEntity add_edge(EdgeBundle&&, LabelEntity&&);
    BeingCreatedEntity add_label(LabelEntity&&, PositionComponent&&);
    BeingCreatedEntity add_label(LabelEntity&&, Entity&&);

    Batch() = default;
    Batch(Batch&&) noexcept = default;
    Batch& operator=(Batch&&) noexcept = default;
    Batch(const Batch&) = delete;

    friend class Scene;
  };

}

#endif 
