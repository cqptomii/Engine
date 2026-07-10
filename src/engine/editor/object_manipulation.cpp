#include "engine/editor/object_manipulation.hpp"
#include "engine/ecs/components/transform_component.hpp"

namespace {
    constexpr float k_translate_sensitivity = 0.01f;
    constexpr float k_rotate_sensitivity    = 0.005f;
    constexpr float k_scale_sensitivity     = 0.01f;
}


void apply_manipulation(
    Scene& scene,
    const std::vector<entt::entity>& entities,
    ManipulationMode mode,
    float delta_x,
    float delta_y)
{
    // Check if the entities are empty or the manipulation mode is NONE
    if (entities.empty() || mode == ManipulationMode::NONE)
    {
        return;
    }

    // Check if the delta x and delta y are 0
    if (delta_x == 0.0f && delta_y == 0.0f)
    {
        return;
    }

    // Get the registry
    Registry& registry = scene.get_registry();
    entt::registry& entt_registry = registry.raw();

    // If there is several entities, calculate the local origin point 

    for (const entt::entity entity : entities)
    {
        // Check if the entity is valid and has a TransformComponent
        if (!entt_registry.valid(entity) || !entt_registry.all_of<TransformComponent>(entity))
        {
            continue;
        }
        // Get the transform component
        TransformComponent& transform = registry.get<TransformComponent>(entity);
        
        // Get the local basis
        const glm::mat3 local_basis = glm::mat3_cast(transform.get_rotation());
        const glm::vec3 local_x = local_basis * glm::vec3(1.0f, 0.0f, 0.0f);
        const glm::vec3 local_y = local_basis * glm::vec3(0.0f, 1.0f, 0.0f);
        
        
        // Apply the manipulation based on the mode
        switch (mode)
        {
            case ManipulationMode::TRANSLATE:
            {
                const glm::vec3 world_delta = local_x * (delta_x * k_translate_sensitivity) + local_y * (-delta_y * k_translate_sensitivity);
                
                // Translate the entity
                transform.translate(world_delta);
                break;
            }
            case ManipulationMode::ROTATE:
            {
                const glm::quat delta_rotation =  glm::angleAxis(-delta_y * k_rotate_sensitivity, local_x) * glm::angleAxis( delta_x * k_rotate_sensitivity, local_y);
                
                // Rotate the entity
                transform.rotate(delta_rotation);
                break;
            }
            case ManipulationMode::SCALE_UNIFORM:
            {
                const float scale_factor = 1.0f - delta_y * k_scale_sensitivity;
                if (scale_factor > 0.0f)
                {
                    // Scale the entity
                    transform.scale_uniform(scale_factor);
                }
                break;
            }
            case ManipulationMode::NONE:
            default:
                break;
        }
    }
}