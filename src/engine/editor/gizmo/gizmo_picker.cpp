#include "engine/editor/gizmo/gizmo_picker.hpp"

#include <array>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace
{
    // Keep this in sync with TransformGizmoRenderer::k_gizmo_screen_scale.
    constexpr float k_gizmo_screen_scale = 0.15f;

    float gizmo_world_size(const SelectionContext& ctx, const CameraData& camera)
    {
        const float distance = glm::length(camera.position - ctx.pivot_world);
        return glm::max(distance, 0.001f) * k_gizmo_screen_scale;
    }

    std::array<glm::vec3, 3> axis_directions(const SelectionContext& ctx)
    {
        const glm::mat3 basis = glm::mat3_cast(ctx.orientation);
        return {
            glm::normalize(basis * glm::vec3(1.0f, 0.0f, 0.0f)),
            glm::normalize(basis * glm::vec3(0.0f, 1.0f, 0.0f)),
            glm::normalize(basis * glm::vec3(0.0f, 0.0f, 1.0f))
        };
    }

    // Shortest distance between a ray (origin + t*dir, t >= 0) and a segment [p0, p1].
    float ray_segment_distance(const glm::vec3& ro, const glm::vec3& rd,
                               const glm::vec3& p0, const glm::vec3& p1,
                               float& out_ray_t)
    {
        const glm::vec3 u = rd;
        const glm::vec3 v = p1 - p0;
        const glm::vec3 w0 = ro - p0;

        const float a = glm::dot(u, u);
        const float b = glm::dot(u, v);
        const float c = glm::dot(v, v);
        const float d = glm::dot(u, w0);
        const float e = glm::dot(v, w0);
        const float denom = a * c - b * b;

        float seg_t = 0.0f;
        if (denom > 1e-6f)
        {
            seg_t = (a * e - b * d) / denom;
        }
        seg_t = glm::clamp(seg_t, 0.0f, 1.0f);

        const glm::vec3 seg_point = p0 + v * seg_t;
        float ray_t = glm::dot(u, seg_point - ro) / a;
        ray_t = glm::max(ray_t, 0.0f);

        const glm::vec3 ray_point = ro + u * ray_t;
        out_ray_t = ray_t;
        return glm::length(ray_point - seg_point);
    }

    GizmoAxis pick_axis_handles(const Ray& ray, const SelectionContext& ctx, const CameraData& camera)
    {
        const float size = gizmo_world_size(ctx, camera);
        const float threshold = size * 0.2f;
        const auto dirs = axis_directions(ctx);

        GizmoAxis best = GizmoAxis::NONE;
        float best_t = std::numeric_limits<float>::max();

        for (int i = 0; i < 3; ++i)
        {
            const glm::vec3 p0 = ctx.pivot_world;
            const glm::vec3 p1 = ctx.pivot_world + dirs[i] * size;

            float ray_t = 0.0f;
            const float distance = ray_segment_distance(ray.getOrigin(), ray.getDirection(), p0, p1, ray_t);

            if (distance < threshold && ray_t < best_t)
            {
                best_t = ray_t;
                best = static_cast<GizmoAxis>(i);
            }
        }

        return best;
    }

    GizmoAxis pick_rings(const Ray& ray, const SelectionContext& ctx, const CameraData& camera)
    {
        const float radius = gizmo_world_size(ctx, camera);
        const float threshold = radius * 0.18f;
        const auto dirs = axis_directions(ctx);

        GizmoAxis best = GizmoAxis::NONE;
        float best_t = std::numeric_limits<float>::max();

        const glm::vec3 ro = ray.getOrigin();
        const glm::vec3 rd = ray.getDirection();

        for (int i = 0; i < 3; ++i)
        {
            const glm::vec3 normal = dirs[i];
            const float denom = glm::dot(rd, normal);
            if (glm::abs(denom) < 1e-5f)
            {
                continue;
            }

            const float t = glm::dot(ctx.pivot_world - ro, normal) / denom;
            if (t < 0.0f)
            {
                continue;
            }

            const glm::vec3 hit = ro + rd * t;
            const float dist_to_pivot = glm::length(hit - ctx.pivot_world);

            if (glm::abs(dist_to_pivot - radius) < threshold && t < best_t)
            {
                best_t = t;
                best = static_cast<GizmoAxis>(i);
            }
        }

        return best;
    }
}

GizmoAxis pick_gizmo_axis(
    const Ray& ray,
    const SelectionContext& ctx,
    ManipulationMode mode,
    const CameraData& camera)
{
    if (!ctx.valid || mode == ManipulationMode::NONE)
    {
        return GizmoAxis::NONE;
    }

    switch (mode)
    {
        case ManipulationMode::TRANSLATE:
        case ManipulationMode::SCALE_UNIFORM:
        case ManipulationMode::SCALE_NON_UNIFORM:
            return pick_axis_handles(ray, ctx, camera);
        case ManipulationMode::ROTATE:
            return pick_rings(ray, ctx, camera);
        case ManipulationMode::NONE:
        default:
            return GizmoAxis::NONE;
    }
}
