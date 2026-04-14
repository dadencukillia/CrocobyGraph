#include "scene.hpp"

namespace CrocobyGraph {

  Scene::~Scene() {
    clear();
  }

  void Scene::remove(EntityGraphNode* node_ptr) {
    if (!node_ptr) return;

    std::erase_if(edges, [node_ptr](EntityGraphEdge* edge) {
      if (edge->start == node_ptr || edge->end == node_ptr) {
        delete edge;
        return true;
      }
      return false;
    });

    auto it = std::find(nodes.begin(), nodes.end(), node_ptr);
    if (it != nodes.end()) {
      delete *it;
      nodes.erase(it);
    }
  }

  void Scene::remove(EntityGraphEdge* edge_ptr) {
    auto it = std::find(edges.begin(), edges.end(), edge_ptr);
    if (it != edges.end()) {
      delete *it;
      edges.erase(it);
    }
  }

  void Scene::remove(EntityGraphNodeLabel* label_ptr) {
    auto it = std::find(node_labels.begin(), node_labels.end(), label_ptr);
    if (it != node_labels.end()) {
      delete *it;
      node_labels.erase(it);
    }
  }

  void Scene::remove(EntityGraphEdgeLabel* label_ptr) {
    auto it = std::find(edge_labels.begin(), edge_labels.end(), label_ptr);
    if (it != edge_labels.end()) {
      delete *it;
      edge_labels.erase(it);
    }
  }

  void Scene::remove(EntityGraphFreeLabel* label_ptr) {
    auto it = std::find(free_labels.begin(), free_labels.end(), label_ptr);
    if (it != free_labels.end()) {
      delete *it;
      free_labels.erase(it);
    }
  }

  void Scene::append(EntitiesBatch&& batch) {
    std::move(batch.nodes.begin(), batch.nodes.end(), std::back_inserter(nodes));
    std::move(batch.edges.begin(), batch.edges.end(), std::back_inserter(edges));
    std::move(batch.node_labels.begin(), batch.node_labels.end(), std::back_inserter(node_labels));
    std::move(batch.edge_labels.begin(), batch.edge_labels.end(), std::back_inserter(edge_labels));
    std::move(batch.free_labels.begin(), batch.free_labels.end(), std::back_inserter(free_labels));

    batch.nodes.clear();
    batch.edges.clear();
    batch.node_labels.clear();
    batch.edge_labels.clear();
    batch.free_labels.clear();
  }

  void Scene::clear() {
    for (auto n : nodes) delete n;
    for (auto e : edges) delete e;
    for (auto l : node_labels) delete l;
    for (auto l : edge_labels) delete l;
    for (auto l : free_labels) delete l;
  }

  const std::vector<EntityGraphNode*>& Scene::get_nodes() const { return nodes; }
  const std::vector<EntityGraphEdge*>& Scene::get_edges() const { return edges; }
  const std::vector<EntityGraphNodeLabel*>& Scene::get_node_labels() const { return node_labels; }
  const std::vector<EntityGraphEdgeLabel*>& Scene::get_edge_labels() const { return edge_labels; }
  const std::vector<EntityGraphFreeLabel*>& Scene::get_free_labels() const { return free_labels; }

}
