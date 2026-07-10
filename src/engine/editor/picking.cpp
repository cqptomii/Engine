#include "engine/editor/picking.hpp"

#include <limits>

#include "engine/scene/Scene.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/ecs/components/mesh_component.hpp"
#include "engine/resources/cpu/mesh_resource.hpp"
#include "engine/math/aabb.hpp"

PickingResult pick_closest_entity(Scene& scene, const Ray& ray)
{
    PickingResult result;
    float closest_distance = std::numeric_limits<float>::max();

    Registry& registry = scene.get_registry();
    CpuResourceManager& resource_manager = scene.get_resource_manager();

    registry.view<TransformComponent, MeshComponent>(
        [&](entt::entity entity, TransformComponent& transform, MeshComponent& mesh_component)
        {
            const ResourceHandle<MeshResource> mesh_handle = mesh_component.get_mesh();
            if (!mesh_handle)
            {
                return;
            }

            MeshResource& mesh_resource = resource_manager.get_mesh(mesh_handle);
            const AABB world_aabb = mesh_resource.get_local_bounds().transform_to_world(transform.get_model_matrix());

            float hit_distance = 0.0f;
            if (!world_aabb.intersectsRay(ray, hit_distance))
            {
                return;
            }

            if (hit_distance > 0.0f && hit_distance < closest_distance)
            {
                closest_distance = hit_distance;
                result.hit = true;
                result.entity = entity;
                result.distance = hit_distance;
            }
        }
    );

    return result;
}
