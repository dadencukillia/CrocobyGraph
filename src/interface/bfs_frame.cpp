#include "bfs_frame.hpp"
#include "../internal/resource_counter.hpp"
#include "../internal/resources.hpp"
#include "ecs.hpp"
#include "entities.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entt.hpp"
#include "imgui.h"
#include <cstdint>

namespace CrocobyGraph {

  uint64_t BFSFrame::generate_scene_hash(GraphECS& ecs) const {
    const auto& registry { ecs.get_scene().get_registry() };
    uint64_t hash { 0 };

    for (const auto& [entity, edge] : registry.view<const EdgeEntity>().each()) {
      uint64_t edge_hash { (static_cast<uint64_t>(edge.node_start) << 32) | static_cast<uint64_t>(edge.node_end) };
      const uint64_t flags { (static_cast<uint64_t>(edge.arrow_on_start) << 1) | static_cast<uint64_t>(edge.arrow_on_end) };

      edge_hash ^= (flags + 1) * 0x9E3779B97F4A7C15ULL;
      edge_hash = (edge_hash ^ (edge_hash >> 30)) * 0xbf58476d1ce4e5b9ULL;
      edge_hash = (edge_hash ^ (edge_hash >> 27)) * 0x94d049bb133111ebULL;
      edge_hash = edge_hash ^ (edge_hash >> 31);
      hash += edge_hash;
    }

    return hash;
  }

  void BFSFrame::load(GraphECS& ecs) {
    ResourceCounter<MediaControlIcons>::inc();
    ResourceCounter<FlagCircleIcon>::inc();
    ResourceCounter<StarCircleIcon>::inc();
  }

  BFSFrame::~BFSFrame() {
    ResourceCounter<MediaControlIcons>::dec();
    ResourceCounter<FlagCircleIcon>::dec();
    ResourceCounter<StarCircleIcon>::dec();
  }

  void BFSFrame::draw(const WindowInfo& info, GraphECS& ecs) {
    ResourceCounter<MediaControlIcons> media_control_icons_res;
    const auto& media_control_icons { media_control_icons_res.get() };

    ImGui::Begin("BFS", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (start_node == entt::null) {
      ImGui::Button("Choose start node");
    } else {
      ImGui::Button("Clear start node");
    }

    if (finish_node == entt::null) {
      ImGui::Button("Choose finish node");
    } else {
      ImGui::Button("Clear finish node");
    }

    ImGui::Checkbox("Show visualization", &show_visualization);
    ImGui::Button("Reset");

    ImGui::Text("Frame: %d", frame);

    ImGui::ImageButton("seek_back", static_cast<ImTextureID>(media_control_icons.back_icon.id), ImVec2 { 24.0f, 24.0f });
    ImGui::SameLine();
    if (paused) {
      ImGui::ImageButton("play", static_cast<ImTextureID>(media_control_icons.play_icon.id), ImVec2 { 24.0f, 24.0f });
    } else {
      ImGui::ImageButton("pause", static_cast<ImTextureID>(media_control_icons.pause_icon.id), ImVec2 { 24.0f, 24.0f });
    }
    ImGui::SameLine();
    ImGui::ImageButton("seek_forward", static_cast<ImTextureID>(media_control_icons.forward_icon.id), ImVec2 { 24.0f, 24.0f });

    ImGui::End();
  }

}
