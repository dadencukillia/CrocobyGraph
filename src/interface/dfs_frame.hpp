#ifndef _CGRAPH_INTERFACE_DFS_FRAME_HPP_
#define _CGRAPH_INTERFACE_DFS_FRAME_HPP_

#include "adjacency_matrix.hpp"
#include "ecs.hpp"
#include "entt/entity/fwd.hpp"
#include "window_system.hpp"
#include <cstdint>
#include <stack>

namespace CrocobyGraph {

  enum class DFSTopologyState : uint8_t {
    NotSet,
    Invalid,
    Valid
  };

  enum class StackAction : bool {
    Pop,
    Push
  };

  class DFSFrame : public WindowUIFrame {
    AdjacencyMatrix prev_matrix { 0 };
    std::stack<entt::entity> stack;
    std::vector<StackAction> log;
    std::vector<entt::entity> visited_nodes;
    entt::entity start_node { 0xFFFFFFFF };
    uint32_t frame { 0 };
    float visualization_time { 0.0f };
    DFSTopologyState topology_state { DFSTopologyState::NotSet };
    bool show_visualization { true };
    bool paused { true };
    bool start_node_selection { false };

    void reset();

  public:
    DFSFrame() = default;
    ~DFSFrame();

    DFSFrame(const DFSFrame&) = delete;
    DFSFrame& operator=(const DFSFrame&) = delete;

    void draw_start_selection(const WindowInfo& info, GraphECS& ecs);
    void load(GraphECS& ecs) override;
    void draw(const WindowInfo& info, GraphECS& ecs) override;
  };

}

#endif
