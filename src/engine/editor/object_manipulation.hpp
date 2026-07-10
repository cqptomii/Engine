#ifndef OBJECT_MANIPULATION_HPP
#define OBJECT_MANIPULATION_HPP

#include "engine/editor/manipulation_mode.hpp"
#include <entt/entity/entity.hpp>
#include "engine/scene/scene.hpp"
#include <vector>

/**
 * @brief Apply the manipulation to the entities
 * 
 * @param scene The scene to apply the manipulation to
 * @param entities The entities to apply the manipulation to (the selected entities)
 * @param mode The manipulation mode (translation, rotation, scale)
 * @param delta_x The delta x from the mouse movement
 * @param delta_y The delta y from the mouse movement
 */
void apply_manipulation(Scene& scene, const std::vector<entt::entity>& entities, ManipulationMode mode, float delta_x, float delta_y);

#endif