#ifndef _CGRAPH_INTERFACE_SCENE_HPP_
#define _CGRAPH_INTERFACE_SCENE_HPP_

#include "batch.hpp"
#include "color.hpp"
#include "entities.hpp"
#include "entt/entity/fwd.hpp"
#include "vector.hpp"
#include <string_view>
#include <unordered_set>
#include <vector>

namespace CrocobyGraph {

  class Scene {
    entt::registry* registry;

  public:
    Scene();
    ~Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;

    void clear();
    [[nodiscard]] entt::registry& get_registry();

    std::vector<entt::entity> append(Batch&& batch, float offset_x = 0.0f, float offset_y = 0.0f);
    Scene& operator+=(Batch&& batch);
    [[nodiscard]] PrimitiveType type(entt::entity entity) const;
    [[nodiscard]] Vector pos(entt::entity entity) const;
    // Returns the position of second node connected to the specified edge
    [[nodiscard]] Vector pos_second(entt::entity entity) const;
    void set_pos(entt::entity entity, Vector pos);
    [[nodiscard]] Color color(entt::entity any_entity) const;
    void set_color(entt::entity any_entity, Color color);
    [[nodiscard]] std::string_view label_text(entt::entity label) const;
    void set_label_text(entt::entity label, std::string&& text);
    [[nodiscard]] Vector label_offset(entt::entity label) const;
    void set_label_offset(entt::entity label, Vector offset);
    [[nodiscard]] std::unordered_set<entt::entity> node_neighbors(entt::entity node) const;
    [[nodiscard]] std::unordered_set<entt::entity> node_edges(entt::entity node) const;
    [[nodiscard]] std::unordered_set<entt::entity> node_labels(entt::entity node) const;
    entt::entity node_set_label(entt::entity node, std::string&& text, Color color = DEFAULT_LABEL_COLOR);
    [[nodiscard]] bool nodes_connected(entt::entity node_a, entt::entity node_b) const;
    entt::entity nodes_connect(entt::entity node_a, entt::entity node_b);
    void nodes_disconnect(entt::entity node_a, entt::entity node_b);
    entt::entity node_add_label(entt::entity node, std::string&& text, Color color = DEFAULT_LABEL_COLOR);
    [[nodiscard]] std::pair<entt::entity, entt::entity> edge_nodes(entt::entity edge) const;
    [[nodiscard]] std::unordered_set<entt::entity> edge_labels(entt::entity edge) const;
    entt::entity edge_set_label(entt::entity edge, std::string&& text, Color color = DEFAULT_LABEL_COLOR);
    [[nodiscard]] std::pair<bool, bool> edge_arrows(entt::entity edge) const;
    void set_edge_arrows(entt::entity edge, std::pair<bool, bool> arrows);
    [[nodiscard]] EdgeCurveType edge_curve(entt::entity edge) const;
    void set_edge_curve(entt::entity edge, EdgeCurveType curve_type);
    entt::entity edge_add_label(entt::entity edge, std::string&& text, Color color = DEFAULT_LABEL_COLOR);
    [[nodiscard]] bool is_node(entt::entity entity) const;
    [[nodiscard]] bool is_edge(entt::entity entity) const;
    [[nodiscard]] bool is_any_label(entt::entity entity) const;
    [[nodiscard]] bool is_free_label(entt::entity entity) const;
    [[nodiscard]] bool is_attach_label(entt::entity entity) const;
  };

}

#endif 
