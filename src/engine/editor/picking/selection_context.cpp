#include "engine/editor/picking/selection_context.hpp"

#include "engine/ecs/components/transform_component.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/scene/Scene.hpp"
#include "engine/scene/scene_hierarchy.hpp"

SelectionContext build_selection_context(Scene& scene, const std::vector<entt::entity>& entities)
{
    SelectionContext context;

    if (entities.empty())
    {
        return context;
    }

    Registry& registry = scene.get_registry();
    entt::registry& entt_registry = registry.raw();

    glm::vec3 position_sum{0.0f};
    std::size_t valid_count = 0;
    bool orientation_set = false;

    for (const entt::entity entity : entities)
    {
        if (!entt_registry.valid(entity) || !entt_registry.all_of<TransformComponent>(entity))
        {
            continue;
        }

        position_sum += scene_hierarchy::get_world_position(scene, entity);

        // v1: gizmo frame = rotation of the first valid selected entity (world orientation).
        if (!orientation_set)
        {
            const glm::mat3 world_rotation = glm::mat3(scene_hierarchy::get_world_matrix(scene, entity));
            context.orientation = glm::quat_cast(world_rotation);
            orientation_set = true;
        }

        ++valid_count;
    }

    if (valid_count == 0)
    {
        return context;
    }

    // v1 pivot: centroid of the selected entity positions.
    context.pivot_world = position_sum / static_cast<float>(valid_count);
    context.valid = true;
    return context;
}
