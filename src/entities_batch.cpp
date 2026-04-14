#include "entities_batch.hpp"

EntityGraphNode* EntitiesBatch::add(EntityGraphNode&& node) {
  auto t = new EntityGraphNode(std::move(node));
  nodes.push_back(t);
  return t;
}

EntityGraphEdge* EntitiesBatch::add(EntityGraphEdge&& edge) {
  auto t = new EntityGraphEdge(std::move(edge));
  edges.push_back(t);
  return t;
}

EntityGraphNodeLabel* EntitiesBatch::add(EntityGraphNodeLabel&& label) {
  auto t = new EntityGraphNodeLabel(std::move(label));
  node_labels.push_back(t);
  return t;
}

EntityGraphEdgeLabel* EntitiesBatch::add(EntityGraphEdgeLabel&& label) {
  auto t = new EntityGraphEdgeLabel(std::move(label));
  edge_labels.push_back(t);
  return t;
}

EntityGraphFreeLabel* EntitiesBatch::add(EntityGraphFreeLabel&& label) {
  auto t = new EntityGraphFreeLabel(std::move(label));
  free_labels.push_back(t);
  return t;
}

EntitiesBatch::~EntitiesBatch() {
  for (auto n : nodes) delete n;
  for (auto e : edges) delete e;
  for (auto l : node_labels) delete l;
  for (auto l : edge_labels) delete l;
  for (auto l : free_labels) delete l;
}
