#ifndef _CGRAPH_SCENE_HPP_
#define _CGRAPH_SCENE_HPP_

#include "entities_batch.hpp"
#include "entities.hpp"
#include <vector>

namespace CrocobyGraph {

  class Scene {
    std::vector<EntityGraphNode*> nodes;
    std::vector<EntityGraphEdge*> edges;
    std::vector<EntityGraphNodeLabel*> node_labels;
    std::vector<EntityGraphEdgeLabel*> edge_labels;
    std::vector<EntityGraphFreeLabel*> free_labels;

  public:
    Scene() = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;

    ~Scene();

    void remove(EntityGraphNode* node_ptr);
    void remove(EntityGraphEdge* edge_ptr);
    void remove(EntityGraphNodeLabel* label_ptr);
    void remove(EntityGraphEdgeLabel* label_ptr);
    void remove(EntityGraphFreeLabel* label_ptr);

    void append(EntitiesBatch&& batch);

    void clear();

    const std::vector<EntityGraphNode*>& get_nodes() const;
    const std::vector<EntityGraphEdge*>& get_edges() const;
    const std::vector<EntityGraphNodeLabel*>& get_node_labels() const;
    const std::vector<EntityGraphEdgeLabel*>& get_edge_labels() const;
    const std::vector<EntityGraphFreeLabel*>& get_free_labels() const;
  };

}

#endif 
