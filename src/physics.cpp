#include "physics.hpp"
#include "components.hpp"
#include "entities.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "raylib.h"
#include <cmath>

namespace CrocobyGraph {

  Physics::~Physics() {
    auto& registry = scene->get_registry();

    for (auto [entity, node] : registry.view<VelocityComponent>().each()) {
      registry.remove<VelocityComponent>(entity);
    }
  }

  void Physics::init(bool jelly, Scene* scene, GraphECS* ecs) {
    this->with_jelly = jelly;
    this->scene = scene;
    this->ecs = ecs;
  }

  void Physics::update(double delta) {
    auto& registry = scene->get_registry();

    for (auto [entity, node] : registry.view<NodeEntity>(entt::exclude<VelocityComponent>).each()) {
      registry.emplace<VelocityComponent>(entity, 0.0f, 0.0f);
    }

    update_velocity(delta);
    move(delta);
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

      b_vel.x += force_apply.x / (b_node.radius * b_node.radius);
      b_vel.y += force_apply.y / (b_node.radius * b_node.radius);
      a_vel.x -= force_apply.x / (a_node.radius * a_node.radius);
      a_vel.y -= force_apply.y / (a_node.radius * a_node.radius);
    }

    for (auto [entity, node, pos, velocity] : registry.view<const NodeEntity, const PositionComponent, VelocityComponent>().each()) {
      float mass = node.radius * node.radius;
      Vector2 forces = { 0.0f, 0.0f };

      // Coulomb's law
      for (auto [another_entity, another_node, another_pos] : registry.view<const NodeEntity, const PositionComponent>().each()) {
        if (entity == another_entity) continue;

        Vector2 vector = { pos.x - another_pos.x, pos.y - another_pos.y };
        float distance_square = vector.x * vector.x + vector.y * vector.y + 0.1f;
        float distance = std::sqrt(distance_square);
        float k = 500'000.0f;
        float force = k / (distance_square * distance);
        forces.x += vector.x * force;
        forces.y += vector.y * force;
      }

      // Gravity force
      float g = 0.5f;
      float center_distance = std::sqrt(pos.x * pos.x + pos.y * pos.y);
      Vector2 gravity_direction = { -pos.x / center_distance, -pos.y / center_distance };
      forces.x += gravity_direction.x * g * mass;
      forces.y += gravity_direction.y * g * mass;

      Vector2 acceleration = { forces.x / mass, forces.y / mass };
      velocity.x += acceleration.x;
      velocity.y += acceleration.y;

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

}
