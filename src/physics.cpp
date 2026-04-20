#include "physics.hpp"
#include "physics_system.hpp"
#include "components.hpp"
#include "entities.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "raylib.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

namespace CrocobyGraph {

  Physics::~Physics() {
    auto& registry = scene->get_registry();

    for (auto [entity, node] : registry.view<VelocityComponent>().each()) {
      registry.remove<VelocityComponent>(entity);
      registry.remove<JellyComponent>(entity);
      registry.remove<RepulsionComponent>(entity);
    }
  }

  void Physics::init(bool jelly, Scene* scene, GraphECS* ecs) {
    this->with_jelly = jelly;
    this->scene = scene;
    this->ecs = ecs;

    for (size_t i = 0; i < jelly_points; ++i) {
      float angle = static_cast<float>(i) / jelly_points * 2.0f * PI;
      jelly_ideal_points[i] = { std::cos(angle), std::sin(angle) };
    }

  }

  void Physics::update(double delta) {
    move(delta);
    if (with_jelly) update_jelly(delta);
    auto& registry = scene->get_registry();

    for (auto [entity, node] : registry.view<const NodeEntity>(entt::exclude<VelocityComponent>).each()) {
      registry.emplace<VelocityComponent>(entity, 0.0f, 0.0f);
    }

    for (auto [entity, node] : registry.view<const NodeEntity>(entt::exclude<RepulsionComponent>).each()) {
      registry.emplace<RepulsionComponent>(entity, 1.0f);
    }

    update_velocity(delta);
  }

  void Physics::update_velocity(double delta) {
    auto& registry = scene->get_registry();

    // Hooke's law
    for (auto [edge_entity, edge] : registry.view<const EdgeEntity>().each()) {
      auto [a_node, a_pos, a_vel] = registry.get<const NodeEntity, const PositionComponent, VelocityComponent>(edge.node_start);
      auto [b_node, b_pos, b_vel] = registry.get<const NodeEntity, const PositionComponent, VelocityComponent>(edge.node_end);

      Vector2 vector = { a_pos.x - b_pos.x, a_pos.y - b_pos.y };
      float distance_square = vector.x * vector.x + vector.y * vector.y + 0.1f;
      float distance = std::sqrt(distance_square);
      float k = 5.0f;
      float ideal_distance = 200.0f;
      float force = k * (distance - ideal_distance);
      Vector2 force_apply = {
        vector.x / distance * force,
        vector.y / distance * force
      };

      b_vel.x += delta * 60.0f * force_apply.x / (b_node.radius * b_node.radius);
      b_vel.y += delta * 60.0f * force_apply.y / (b_node.radius * b_node.radius);
      a_vel.x -= delta * 60.0f * force_apply.x / (a_node.radius * a_node.radius);
      a_vel.y -= delta * 60.0f * force_apply.y / (a_node.radius * a_node.radius);
    }

    for (auto [entity, node, pos, repulsion, velocity] : registry.view<const NodeEntity, const PositionComponent, const RepulsionComponent, VelocityComponent>().each()) {
      float mass = node.radius * node.radius;
      Vector2 forces = { 0.0f, 0.0f };

      // Coulomb's law
      for (auto [another_entity, another_repulsion, another_pos] : registry.view<const RepulsionComponent, const PositionComponent>().each()) {
        if (entity == another_entity) continue;

        Vector2 vector = { pos.x - another_pos.x, pos.y - another_pos.y };
        float distance_square = vector.x * vector.x + vector.y * vector.y + 0.1f;
        float distance = std::sqrt(distance_square);
        float k = 1'000'000.0f;
        float force = k * another_repulsion.charge * repulsion.charge / (distance_square * distance);
        forces.x += vector.x * force;
        forces.y += vector.y * force;
      }

      // Gravity force
      float g = 1.0f;
      float center_distance = std::sqrt(pos.x * pos.x + pos.y * pos.y) + 0.1f;
      Vector2 gravity_direction = { -pos.x / center_distance, -pos.y / center_distance };
      forces.x += gravity_direction.x * g * mass;
      forces.y += gravity_direction.y * g * mass;

      Vector2 acceleration = { forces.x / mass, forces.y / mass };
      velocity.x += acceleration.x * delta * 60.0f;
      velocity.y += acceleration.y * delta * 60.0f;

      float friction = 1.5f;
      velocity.x -= velocity.x * friction * delta;
      velocity.y -= velocity.y * friction * delta;
    }
  }

  void Physics::move(double delta) {
    auto& registry = scene->get_registry();

    for (auto [entity, pos, velocity] : registry.view<PositionComponent, const VelocityComponent>().each()) {
      pos.x += velocity.x * delta;
      pos.y += velocity.y * delta;
    }
  }

  void Physics::update_jelly(double delta) {
    auto& registry = scene->get_registry();
    std::vector<entt::entity> jelly_to_free;

    // Make node with velocity be jelly
    for (auto [entity, node, pos, velocity] : registry.view<const NodeEntity, const PositionComponent, const VelocityComponent>(entt::exclude<JellyComponent>).each()) {
      if (velocity.x * velocity.x + velocity.y * velocity.y >= 900.0f) {
        std::vector<PositionComponent> points(jelly_points);
        for (size_t i = 0; i < jelly_points; ++i) {
          points[i] = { pos.x + jelly_ideal_points[i].x * static_cast<float>(node.radius), pos.y + jelly_ideal_points[i].y * static_cast<float>(node.radius) };
        }

        registry.emplace<JellyComponent>(entity, std::move(points));
      }
    }

    // Move jelly points
    float threshold = 0.5f;
    for (auto [entity, node, pos, velocity, jelly] : registry.view<const NodeEntity, const PositionComponent, const VelocityComponent, JellyComponent>().each()) {
      float vel_value = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y) + 0.01f;
      Vector2 vel_dir = { velocity.x / vel_value, velocity.y / vel_value };
      bool is_nearly_ideal = true;

      for (size_t i = 0; i < jelly_points; ++i) {
        auto& point_dir = jelly_ideal_points[i];
        float diff = vel_dir.x * point_dir.x + vel_dir.y * point_dir.y;
        float impact_mul = (diff + 1.0f) / 2.0f;
        float total_mul = (impact_mul * impact_mul + 0.05f) * delta * 60;
        Vector2 ideal_pos = { 
          pos.x + point_dir.x * static_cast<float>(node.radius),
          pos.y + point_dir.y * static_cast<float>(node.radius)
        };
        float squish_factor = vel_value * 0.02f;
        Vector2 deformed_ideal = {
          pos.x + point_dir.x * (static_cast<float>(node.radius) - diff * squish_factor),
          pos.y + point_dir.y * (static_cast<float>(node.radius) - diff * squish_factor)
        };

        auto& jelly_point = jelly.points[i];
        jelly_point.x = (jelly_point.x + total_mul * ideal_pos.x) / (1 + total_mul);
        jelly_point.y = (jelly_point.y + total_mul * ideal_pos.y) / (1 + total_mul);

        float dist_to_ideal = std::abs(jelly_point.x - ideal_pos.x) + std::abs(jelly_point.y - ideal_pos.y);
        if (dist_to_ideal > threshold) {
          is_nearly_ideal = false;
        }
      }

      if (is_nearly_ideal && vel_value < 5.0f) jelly_to_free.push_back(entity);
    }

    // Remove slow nodes jelly
    for (auto jelly : jelly_to_free) {
      registry.erase<JellyComponent>(jelly);
    }
  }

}
