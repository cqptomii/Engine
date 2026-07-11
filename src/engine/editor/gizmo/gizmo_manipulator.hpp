#ifndef GIZMO_MANIPULATOR_HPP
#define GIZMO_MANIPULATOR_HPP

#include <vector>

#include <entt/entity/entity.hpp>
#include <glm/glm.hpp>

#include "engine/editor/gizmo/gizmo_axis.hpp"
#include "engine/editor/manipulation_mode.hpp"
#include "engine/editor/selection_context.hpp"
#include "engine/math/ray.hpp"
#include "engine/rendering/utils/camera_data.hpp"

class Scene;

/**
 * @brief Apply a gizmo drag constrained to a single axis.
 *
 * The mouse delta is projected onto the on-screen direction of the grabbed axis
 * so the manipulation follows the cursor. Translate moves along the axis,
 * rotate spins around it, scale stretches the matching local component.
 *
 * @param scene    Scene owning the selected entities.
 * @param entities Selected entities to transform.
 * @param mode     Active manipulation mode.
 * @param axis     Grabbed gizmo axis (X / Y / Z).
 * @param ctx      Selection pivot + orientation (gizmo frame).
 * @param camera   Camera used to project the axis to screen space.
 * @param delta_x  Accumulated mouse delta on X (pixels).
 * @param delta_y  Accumulated mouse delta on Y (pixels).
 */
void apply_gizmo_manipulation(
    Scene& scene,
    const std::vector<entt::entity>& entities,
    ManipulationMode mode,
    GizmoAxis axis,
    const SelectionContext& ctx,
    const CameraData& camera,
    float delta_x,
    float delta_y);

/**
 * @brief Intersect a ray with the plane of the given rotation ring.
 *
 * The ring plane passes through the pivot with normal = the axis world
 * direction. On success, @p out_dir is the normalized vector from the pivot to
 * the hit point (used to measure the swept rotation angle).
 *
 * @return false when the ray is parallel to the plane or hits behind the pivot.
 */
bool gizmo_ring_plane_dir(
    const Ray& ray,
    const SelectionContext& ctx,
    GizmoAxis axis,
    glm::vec3& out_dir);

/**
 * @brief Apply a rotation gizmo drag from the swept angle between two ring-plane
 *        directions (previous and current), rotating around the axis.
 */
void apply_gizmo_rotation_drag(
    Scene& scene,
    const std::vector<entt::entity>& entities,
    GizmoAxis axis,
    const SelectionContext& ctx,
    const glm::vec3& prev_dir,
    const glm::vec3& cur_dir);

#endif // GIZMO_MANIPULATOR_HPP
