#ifndef _CGRAPH_INTERFACE_BFS_FRAME_HPP_
#define _CGRAPH_INTERFACE_BFS_FRAME_HPP_

#include "entt/entity/fwd.hpp"
#include "window_system.hpp"
#include <cstdint>
#include <queue>

namespace CrocobyGraph {

  class BFSFrame : public WindowUIFrame {
    std::queue<std::pair<uint32_t, entt::entity>> queue;
    uint64_t scene_hash { 0 };
    entt::entity start_node { 0xFFFFFFFF };
    entt::entity finish_node { 0xFFFFFFFF };
    uint32_t frame { 0 };
    float visualization_time { 0.0f };
    bool show_visualization { true };
    bool paused { true };

    [[nodiscard]] uint64_t generate_scene_hash(GraphECS& ecs) const;

  public:
    BFSFrame() = default;
    ~BFSFrame();

    BFSFrame(const BFSFrame&) = delete;
    BFSFrame& operator=(const BFSFrame&) = delete;

    void load(GraphECS& ecs) override;
    void draw(const WindowInfo& info, GraphECS& ecs) override;
  };

}

#endif
