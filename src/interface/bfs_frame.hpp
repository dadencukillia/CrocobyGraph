#ifndef _CGRAPH_INTERFACE_BFS_FRAME_HPP_
#define _CGRAPH_INTERFACE_BFS_FRAME_HPP_

#include "ecs.hpp"
#include "entt/entity/fwd.hpp"
#include "window_system.hpp"
#include <cstdint>
#include <queue>

namespace CrocobyGraph {

  class BFSFrame : public WindowUIFrame {
    std::queue<std::pair<uint32_t, entt::entity>> queue;
    std::vector<uint32_t> visited_orders;
    std::vector<entt::entity> visited_nodes;
    std::vector<std::string> labels_pool;
    std::vector<entt::entity> saved_path;
    uint64_t scene_hash { 0 };
    entt::entity start_node { 0xFFFFFFFF };
    entt::entity finish_node { 0xFFFFFFFF };
    uint32_t frame { 0 };
    float visualization_time { 0.0f };
    bool show_visualization { true };
    bool paused { true };
    bool start_node_selection { false };
    bool finish_node_selection { false };

    [[nodiscard]] uint64_t generate_scene_hash(GraphECS& ecs) const;
    void reset();

  public:
    BFSFrame() = default;
    ~BFSFrame();

    BFSFrame(const BFSFrame&) = delete;
    BFSFrame& operator=(const BFSFrame&) = delete;

    void draw_start_selection(const WindowInfo& info, GraphECS& ecs);
    void draw_finish_selection(const WindowInfo& info, GraphECS& ecs);
    void load(GraphECS& ecs) override;
    void draw(const WindowInfo& info, GraphECS& ecs) override;
  };

}

#endif
