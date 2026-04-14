#ifndef _CGRAPH_ENTITIES_BATCH_HPP_
#define _CGRAPH_ENTITIES_BATCH_HPP_

#include "entities.hpp"
#include <vector>

namespace CrocobyGraph {

  class EntitiesBatch {
    std::vector<EntityGraphNode*> nodes;
    std::vector<EntityGraphEdge*> edges;
    std::vector<EntityGraphNodeLabel*> node_labels;
    std::vector<EntityGraphEdgeLabel*> edge_labels;
    std::vector<EntityGraphFreeLabel*> free_labels;

  public:
    EntityGraphNode* add(EntityGraphNode&& node);
    EntityGraphEdge* add(EntityGraphEdge&& edge);
    EntityGraphNodeLabel* add(EntityGraphNodeLabel&& label);
    EntityGraphEdgeLabel* add(EntityGraphEdgeLabel&& label);
    EntityGraphFreeLabel* add(EntityGraphFreeLabel&& label);

    EntitiesBatch(EntitiesBatch&&) noexcept = default;
    EntitiesBatch& operator=(EntitiesBatch&&) noexcept = default;
    EntitiesBatch(const EntitiesBatch&) = delete;

    ~EntitiesBatch();

    friend class Scene;
  };

}

#endif 
