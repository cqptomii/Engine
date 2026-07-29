#include "engine/editor/gizmo/gizmo_manipulator.hpp"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "engine/ecs/components/transform_component.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/scene/Scene.hpp"
#include "engine/scene/scene_hierarchy.hpp"

namespace
{
    // The translate sensitivity
    constexpr float k_translate_sensitivity = 0.0025f;
    constexpr float k_scale_sensitivity     = 0.01f;

    /**
     * @brief Method to get the world direction of an axis
     * 
     * @param ctx The selection context
     * @param axis The axis
     * @return glm::vec3 The world direction of the axis
     */
    glm::vec3 axis_world_direction(const SelectionContext& ctx, GizmoAxis axis)
    {
        // Get the basis of the orientation
        const glm::mat3 basis = glm::mat3_cast(ctx.orientation);
        // Return the world direction of the axis
        switch (axis)
        {
            case GizmoAxis::X:  return glm::normalize(basis * glm::vec3(1.0f, 0.0f, 0.0f));
            case GizmoAxis::Y: return glm::normalize(basis * glm::vec3(0.0f, 1.0f, 0.0f));
            case GizmoAxis::Z: return glm::normalize(basis * glm::vec3(0.0f, 0.0f, 1.0f));
            default:           return glm::vec3(0.0f);
        }
    }

    /**
     * @brief Method to get the screen amount along an axis
     * 
     * @param ctx The selection context
     * @param axis_world The world direction of the axis
     * @param camera The camera data
     * @param delta_x The delta x
     * @param delta_y The delta y
     * @return float The screen amount along the axis
     */
    float screen_amount_along_axis(const SelectionContext& ctx,const glm::vec3& axis_world, const CameraData& camera, float delta_x, float delta_y)
    {
        // Get the view projection matrix
        const glm::mat4 view_proj = camera.projection * camera.view;

        // Get the clip space positions of the pivot
        const glm::vec4 clip0 = view_proj * glm::vec4(ctx.pivot_world, 1.0f);
        const glm::vec4 clip1 = view_proj * glm::vec4(ctx.pivot_world + axis_world, 1.0f);

        if (glm::abs(clip0.w) < 1e-5f || glm::abs(clip1.w) < 1e-5f)
        {
            // If the pivot is too close to the camera, return the delta x
            return delta_x;
        }

        // Get the NDC positions of the pivot
        const glm::vec2 ndc0 = glm::vec2(clip0) / clip0.w;
        const glm::vec2 ndc1 = glm::vec2(clip1) / clip1.w;

        // Get the screen direction
        glm::vec2 screen_dir = ndc1 - ndc0;
        if (glm::length(screen_dir) < 1e-6f)
        {
            return delta_x;
        }
        screen_dir = glm::normalize(screen_dir);

        // Mouse delta in NDC-like space (screen Y grows downward, NDC Y grows up).
        const glm::vec2 mouse_delta(delta_x, -delta_y);
        return glm::dot(mouse_delta, screen_dir);
    }
}


/**
 * @brief Method to apply the gizmo manipulation
 * 
 * @param scene The scene
 * @param entities The entities
 * @param mode The manipulation mode
 * @param axis The axis
 * @param ctx The selection context
 * @param camera The camera data
 * @param delta_x The delta x
 * @param delta_y The delta y
 */
void apply_gizmo_manipulation(Scene& scene, const std::vector<entt::entity>& entities, ManipulationMode mode, GizmoAxis axis,  const SelectionContext& ctx, const CameraData& camera, float delta_x, float delta_y)
{
    // If the entities are empty, the axis is none, or the mode is none, return
    if (entities.empty() || axis == GizmoAxis::NONE || mode == ManipulationMode::NONE)
    {
        return;
    }

    // If the delta x and delta y are 0, return
    if (delta_x == 0.0f && delta_y == 0.0f)
    {
        return;
    }

    // Get the world direction of the axis
    const glm::vec3 axis_world = axis_world_direction(ctx, axis);
    if (glm::length(axis_world) < 1e-6f)
    {
        return;
    }

    // Get the screen amount along the axis
    const float screen_amount = screen_amount_along_axis(ctx, axis_world, camera, delta_x, delta_y);
    const float distance = glm::max(glm::length(camera.position - ctx.pivot_world), 0.001f);
    const int axis_index = static_cast<int>(axis);

    // Get the registry
    Registry& registry = scene.get_registry();
    // Get the raw registry
    entt::registry& entt_registry = registry.raw();

    for (const entt::entity entity : entities)
    {
        if (!entt_registry.valid(entity) || !entt_registry.all_of<TransformComponent>(entity))
        {
            continue;
        }

        TransformComponent& transform = registry.get<TransformComponent>(entity);

        // Apply the manipulation based on the mode
        switch (mode)
        {
            case ManipulationMode::TRANSLATE:
            {
                // The axis and the drag delta are both world-space, so the move must be
                // applied through the hierarchy: TransformComponent::translate() works in
                // local space and would drift under a rotated or scaled parent.
                const glm::vec3 world_delta = axis_world * (screen_amount * distance * k_translate_sensitivity);
                const glm::vec3 world_position = scene_hierarchy::get_world_position(scene, entity);
                scene_hierarchy::set_world_position(scene, entity, world_position + world_delta);
                break;
            }
            case ManipulationMode::ROTATE:
                // Rotation drag is handled by apply_gizmo_rotation_drag (ring-plane
                // angle sweep), which is accurate regardless of the viewing angle.
                break;
            case ManipulationMode::SCALE_UNIFORM:
            case ManipulationMode::SCALE_NON_UNIFORM:
            {
                const float factor = 1.0f + screen_amount * k_scale_sensitivity;
                if (factor > 0.0f)
                {
                    glm::vec3 scale = transform.get_scale();
                    scale[axis_index] *= factor;
                    transform.set_scale(scale);
                }
                break;
            }
            case ManipulationMode::NONE:
            default:
                break;
        }
    }
}

/**
 * @brief Method to get the direction of the gizmo ring plane
 * 
 * @param ray The ray
 * @param ctx The selection context
 * @param axis The axis
 * @param out_dir The output direction
 * @return true If the direction is valid
 * @return false If the direction is not valid
 */
bool gizmo_ring_plane_dir(const Ray& ray, const SelectionContext& ctx, GizmoAxis axis, glm::vec3& out_dir)
{
    // If the context is not valid or the axis is none, return false
    if (!ctx.valid || axis == GizmoAxis::NONE)
    {
        return false;
    }

    // Get the normal of the axis
    const glm::vec3 normal = axis_world_direction(ctx, axis);
    if (glm::length(normal) < 1e-6f)
    {
        return false;
    }

    // Get the origin and direction of the ray
    const glm::vec3 ro = ray.getOrigin();
    const glm::vec3 rd = ray.getDirection();

    // Get the denominator of the ray
    const float denom = glm::dot(rd, normal);
    if (glm::abs(denom) < 1e-5f)
    {
        return false;
    }

    // Get the time of the ray
    const float t = glm::dot(ctx.pivot_world - ro, normal) / denom;
    if (t < 0.0f)
    {
        return false;
    }

    // Get the hit point of the ray
    const glm::vec3 hit = ro + rd * t;
    const glm::vec3 radial = hit - ctx.pivot_world;
    if (glm::length(radial) < 1e-6f)
    {
        return false;
    }

    out_dir = glm::normalize(radial);
    return true;
}

/**
 * @brief Method to apply the gizmo rotation drag
 * 
 * @param scene The scene
 * @param entities The entities
 * @param axis The axis
 * @param ctx The selection context
 * @param prev_dir The previous direction
 * @param cur_dir The current direction
 */
void apply_gizmo_rotation_drag(Scene& scene, const std::vector<entt::entity>& entities, GizmoAxis axis, const SelectionContext& ctx, const glm::vec3& prev_dir, const glm::vec3& cur_dir)
{
    // If the entities are empty or the axis is none, return
    if (entities.empty() || axis == GizmoAxis::NONE)
    {
        return;
    }

    // Get the world direction of the axis
    const glm::vec3 axis_world = axis_world_direction(ctx, axis);
    if (glm::length(axis_world) < 1e-6f)
    {
        // If the axis world is not valid, return
        return;
    }

    // Signed angle swept from prev_dir to cur_dir around the axis.
    const float angle = std::atan2(
        glm::dot(glm::cross(prev_dir, cur_dir), axis_world),
        glm::dot(prev_dir, cur_dir));

    if (glm::abs(angle) < 1e-6f)
    {
        return;
    }

    const glm::quat delta_rotation = glm::angleAxis(angle, axis_world);

    Registry& registry = scene.get_registry();
    entt::registry& entt_registry = registry.raw();

    for (const entt::entity entity : entities)
    {
        if (!entt_registry.valid(entity) || !entt_registry.all_of<TransformComponent>(entity))
        {
            continue;
        }

        TransformComponent& transform = registry.get<TransformComponent>(entity);
        transform.rotate(delta_rotation);
    }
}
