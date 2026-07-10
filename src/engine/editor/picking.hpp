#ifndef PICKING_HPP
#define PICKING_HPP

#include <entt/entt.hpp>
#include "engine/math/ray.hpp"

class Scene;

/**
 * @brief Picking result struct
 */
struct PickingResult
{
    bool hit = false;
    entt::entity entity = entt::null;
    float distance = 0.0f;
};

/**
 * @brief Pick the closest entity hit by a world-space ray.
 *
 * The ray must already be in world space (built from camera unproject).
 * Each mesh AABB is transformed with the entity model matrix only.
 */
PickingResult pick_closest_entity(Scene& scene, const Ray& ray);

#endif
