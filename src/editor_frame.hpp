#ifndef _CGRAPH_EDITOR_FRAME_HPP_
#define _CGRAPH_EDITOR_FRAME_HPP_

#include "ecs.hpp"
#include "entt/entity/fwd.hpp"
#include "window_system.hpp"
#include <unordered_set>

namespace CrocobyGraph {

  struct IconTextures;

  enum class EditMode {
    View,
    Node,
    Edge,
    Label,
  };

  struct DragState {
    bool dragging { false };
    float start_x { 0.0f };
    float start_y { 0.0f };
    float end_x { 0.0f };
    float end_y { 0.0f };
  };

  class EditorFrame : public WindowUIFrame {
    IconTextures* textures { nullptr };
    EditMode editor_mode { EditMode::View };
    DragState selection_drag {};
    DragState motion_drag {};
    std::unordered_set<entt::entity> selection;
    std::unordered_set<entt::entity> temp_selection;

    void draw_mode_toolbar(bool& toggle_view, bool& toggle_node, bool& toggle_edge, bool& toggle_label, bool current_view, bool current_node, bool current_edge, bool current_label);
    void process_mode_toggle(bool toggle_view, bool toggle_node, bool toggle_edge, bool toggle_label);
    void process_selection(const WindowInfo& info, GraphECS& ecs, bool current_view, bool current_node, bool current_edge, bool current_label);
    void process_motion(const WindowInfo& info, GraphECS& ecs, bool current_view, bool current_node, bool current_edge, bool current_label);

  public:
    EditorFrame() = default;
    EditorFrame(const EditorFrame&) = delete;
    EditorFrame& operator=(const EditorFrame&) = delete;
    ~EditorFrame();

    void load(GraphECS& ecs) override;
    void draw(const WindowInfo& info, GraphECS& ecs) override;
  };

}

#endif
