#ifndef OBJECT_MANIPULATION_HPP
#define OBJECT_MANIPULATION_HPP

#include "engine/editor/manipulation_mode.hpp"
#include <entt/entity/entity.hpp>
#include "engine/scene/scene.hpp"
#include "engine/rendering/utils/camera_data.hpp"
#include <vector>

/**
 * @brief Apply the manipulation to the entities
 *
 * Translate uses the camera plane (view right / up) so the object can be moved
 * in any world direction as the camera orbits, instead of being locked to the
 * object local XY plane. Rotate and scale stay in the object local frame.
 *
 * @param scene The scene to apply the manipulation to
 * @param entities The entities to apply the manipulation to (the selected entities)
 * @param mode The manipulation mode (translation, rotation, scale)
 * @param delta_x The delta x from the mouse movement
 * @param delta_y The delta y from the mouse movement
 * @param camera The camera used to build the translation plane
 */
void apply_manipulation(Scene& scene, const std::vector<entt::entity>& entities, ManipulationMode mode, float delta_x, float delta_y, const CameraData& camera);

#endif