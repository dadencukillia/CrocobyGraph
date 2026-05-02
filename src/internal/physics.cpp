#include "physics.hpp"
#include "../interface/physics_system.hpp"
#include "../interface/components.hpp"
#include "../interface/entities.hpp"
#include "../config.hpp" 
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "math.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include <vector>
#include "raylib.h"

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

    if (jelly) {
      float angle_segment = 2.0f * PI / jelly_points;
      for (size_t i = 0; i < jelly_points; ++i) {
        float angle = static_cast<float>(i) * angle_segment;
        crossplatform_sincos(angle, &jelly_ideal_points[i].y, &jelly_ideal_points[i].x);
      }
    }

    threads.add_thread_pool(ecs->get_threadpool());
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
    for (const auto& [edge_entity, edge] : registry.view<const EdgeEntity>().each()) {
      const auto& [a_node, a_pos, a_vel] = registry.get<const NodeEntity, const PositionComponent, VelocityComponent>(edge.node_start);
      const auto& [b_node, b_pos, b_vel] = registry.get<const NodeEntity, const PositionComponent, VelocityComponent>(edge.node_end);

      Vector2 vector = { a_pos.x - b_pos.x, a_pos.y - b_pos.y };
      float distance_square = vector.x * vector.x + vector.y * vector.y + 0.1f;
      float distance = std::sqrt(distance_square);
      float force = PHYSICS_ATTRACTION_CONSTANT * (distance - PHYSICS_ATTRACTION_IDEAL_DISTANCE);

      float force_apply_denominator = 1.0f / distance * force;
      Vector2 force_apply = {
        vector.x * force_apply_denominator,
        vector.y * force_apply_denominator
      };

      float vel_denominator = 1.0f / (b_node.radius * b_node.radius);
      b_vel.x += delta * TARGET_TPS * force_apply.x * vel_denominator;
      b_vel.y += delta * TARGET_TPS * force_apply.y * vel_denominator;
      a_vel.x -= delta * TARGET_TPS * force_apply.x * vel_denominator;
      a_vel.y -= delta * TARGET_TPS * force_apply.y * vel_denominator;
    }

    constexpr size_t cache_line_size { std::hardware_destructive_interference_size };
    constexpr size_t velocity_type_size { sizeof(VelocityComponent) };
    constexpr size_t chunk_multiple { std::max(1uz, cache_line_size / velocity_type_size) };

    auto& storage { registry.storage<VelocityComponent>() };
    const size_t velocity_count { storage.size() };

    const auto thread_worker = [delta, &registry](const entt::entity* start, const size_t len) {
      for (size_t i = 0; i < len; ++i) {
        const auto entity = start[i];
        const auto& [node, pos, repulsion, velocity] = registry.get<const NodeEntity, const PositionComponent, const RepulsionComponent, VelocityComponent>(entity);

        float mass = node.radius * node.radius;
        Vector2 forces = { 0.0f, 0.0f };

        // Coulomb's law
        for (const auto& [another_entity, another_repulsion, another_pos] : registry.view<const RepulsionComponent, const PositionComponent>().each()) {
          if (entity == another_entity) continue;

          Vector2 vector = { pos.x - another_pos.x, pos.y - another_pos.y };
          float distance_square = vector.x * vector.x + vector.y * vector.y + 0.1f;
          float distance = std::sqrt(distance_square);
          float force = PHYSICS_REPULSION_CONSTANT * another_repulsion.charge * repulsion.charge / (distance_square * distance);
          forces.x += vector.x * force;
          forces.y += vector.y * force;
        }

        // Gravity force
        float center_distance = std::sqrt(pos.x * pos.x + pos.y * pos.y) + 0.1f;
        float inv_center_distance = 1.0f / center_distance;
        Vector2 gravity_direction = { -pos.x * inv_center_distance, -pos.y * inv_center_distance };
        forces.x += gravity_direction.x * PHYSICS_GRAVITY_CONSTANT * mass;
        forces.y += gravity_direction.y * PHYSICS_GRAVITY_CONSTANT * mass;

        // Velocity calculations
        float inv_mass = 1.0f / mass;
        Vector2 acceleration = { forces.x * inv_mass, forces.y * inv_mass };
        velocity.x += acceleration.x * delta * TARGET_TPS;
        velocity.y += acceleration.y * delta * TARGET_TPS;

        velocity.x -= velocity.x * PHYSICS_FRICTION_CONSTANT * delta;
        velocity.y -= velocity.y * PHYSICS_FRICTION_CONSTANT * delta;
      }
    };

    if (velocity_count >= 128 && PHYSICS_CALCULATIONS_THREADS != 0) {
      // Ensures that VelocityComponents will be located in the memory by the id order
      registry.sort<VelocityComponent>([](const entt::entity lhs, const entt::entity rhs) {
        return lhs < rhs;
      });

      const entt::entity* velocity_data { storage.data() };
      const size_t chunk_size { ((velocity_count / PHYSICS_CALCULATIONS_THREADS + chunk_multiple - 1) / chunk_multiple) * chunk_multiple };
      size_t task_ids[PHYSICS_CALCULATIONS_THREADS];

      for (uint8_t thread = 0; thread < PHYSICS_CALCULATIONS_THREADS; ++thread) {
        const entt::entity* start = &velocity_data[chunk_size * thread];
        const size_t len = std::min(chunk_size, velocity_count - chunk_size * thread - 1);
        task_ids[thread] = ecs->get_threadpool().enqueue_task([thread_worker, start, len]() {
          thread_worker(start, len);
        });
      }

      for (uint8_t thread = 0; thread < PHYSICS_CALCULATIONS_THREADS; ++thread) {
        ecs->get_threadpool().wait_for_task(task_ids[thread]);
      }
    } else {
      const entt::entity* velocity_data { storage.data() };
      thread_worker(velocity_data, velocity_count);
    }
  }

  void Physics::move(double delta) {
    auto& registry = scene->get_registry();

    for (const auto& [entity, pos, velocity] : registry.view<PositionComponent, const VelocityComponent>().each()) {
      pos.x += velocity.x * delta;
      pos.y += velocity.y * delta;
    }
  }

  void Physics::update_jelly(double delta) {
    auto& registry = scene->get_registry();
    std::vector<entt::entity> jelly_to_free;

    // Make node with velocity be jelly
    for (const auto& [entity, node, pos, velocity] : registry.view<const NodeEntity, const PositionComponent, const VelocityComponent>(entt::exclude<JellyComponent>).each()) {
      if (velocity.x * velocity.x + velocity.y * velocity.y >= 900.0f) {
        std::vector<PositionComponent> points(jelly_points);
        for (size_t i = 0; i < jelly_points; ++i) {
          points[i] = { pos.x + jelly_ideal_points[i].x * node.radius, pos.y + jelly_ideal_points[i].y * node.radius };
        }

        registry.emplace<JellyComponent>(entity, std::move(points));
      }
    }

    // Move jelly points
    float threshold = 0.5f;
    for (const auto& [entity, node, pos, velocity, jelly] : registry.view<const NodeEntity, const PositionComponent, const VelocityComponent, JellyComponent>().each()) {
      float vel_value = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y) + 0.01f;
      Vector2 vel_dir = { velocity.x / vel_value, velocity.y / vel_value };
      bool is_nearly_ideal = true;

      for (size_t i = 0; i < jelly_points; ++i) {
        auto& point_dir = jelly_ideal_points[i];
        float diff = vel_dir.x * point_dir.x + vel_dir.y * point_dir.y;
        float impact_mul = (diff + 1.0f) / 2.0f;
        float total_mul = (impact_mul * impact_mul + 0.05f) * delta * TARGET_TPS;
        Vector2 ideal_pos = { 
          pos.x + point_dir.x * node.radius,
          pos.y + point_dir.y * node.radius
        };
        float squish_factor = vel_value * 0.02f;
        Vector2 deformed_ideal = {
          pos.x + point_dir.x * (node.radius - diff * squish_factor),
          pos.y + point_dir.y * (node.radius - diff * squish_factor)
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
