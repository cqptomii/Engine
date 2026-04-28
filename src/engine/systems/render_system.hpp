//
// Created by tomfr on 10/03/2026.
//

#ifndef ENGINE_RENDER_SYSTEM_HPP
#define ENGINE_RENDER_SYSTEM_HPP

#include <memory>
#include "engine/scene/Scene.hpp"
#include "engine/rendering/utils/camera_data.hpp"
#include "engine/rendering/render_queue.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/ecs/components/model_component.hpp"
#include "engine/ecs/components/mesh_component.hpp"
#include "engine/ecs/components/material_component.hpp"

#include <engine/rendering/renderer.hpp>
#include <engine/rendering/render_queue.hpp>

class RenderSystem
{
private:
    std::unique_ptr<Renderer> renderer_ptr;
    std::unique_ptr<RenderQueue> render_queue_ptr;
public:
    explicit RenderSystem() {
        this->renderer_ptr = std::make_unique<Renderer>();
        this->render_queue_ptr = std::make_unique<RenderQueue>();
    };
    void update(Scene& scene, const CameraData& camera, bool editor_mode = true)
    {
        // Set the matrix given by the main camera of the scene
        glm::mat4 view, projection;

        // Get the view and projection matrix from the camera data
        view = camera.view;
        projection = camera.projection;

        auto& registry = scene.get_registry();
        auto& resource_manager = scene.get_resource_manager();

        // Iterate over each entity with a TransformComponent and a ModelComponent
        registry.view<TransformComponent, ModelComponent>(
            [&](entt::entity e,TransformComponent& transform, ModelComponent& modelComp)
            {
                ResourceHandle<MaterialInstance> entity_material;
                if (registry.raw().all_of<MaterialComponent>(e))
                {
                    entity_material = registry.raw().get<MaterialComponent>(e).get_material();
                }

                ModelResource& model = resource_manager.get_model(modelComp.get_model());

                for (auto& mesh_handle : model.get_meshes())
                {
                    MeshResource& mesh = resource_manager.get_mesh(mesh_handle);

                    for (auto& submesh : mesh.get_submeshes())
                    {
                        const ResourceHandle<MaterialInstance> selected_material = entity_material ? entity_material : submesh.material;
                        if (!selected_material)
                        {
                            continue;
                        }

                        RenderCommand cmd;
                        cmd.mesh_handle = mesh_handle;
                        cmd.mesh = &mesh;
                        cmd.material = &resource_manager.get_material_instance(selected_material);

                        // Set Transformation Matrix to get the relative position of the model to the main camera
                        cmd.transform = transform.get_model_matrix();
                        cmd.view = view;
                        cmd.projection = projection;

                        this->render_queue_ptr->push(cmd);
                    }
                }
            }
        );

        registry.view<TransformComponent, MeshComponent>(
            [&](entt::entity e, TransformComponent& transform, MeshComponent& meshComp)
            {
                if (!registry.raw().all_of<MaterialComponent>(e))
                {
                    return;
                }

                const ResourceHandle<MaterialInstance> material_handle = registry.raw().get<MaterialComponent>(e).get_material();
                if (!material_handle)
                {
                    return;
                }

                const ResourceHandle<MeshResource> mesh_handle = meshComp.get_mesh();
                MeshResource& mesh = resource_manager.get_mesh(mesh_handle);

                RenderCommand cmd;
                cmd.mesh_handle = mesh_handle;
                cmd.mesh = &mesh;
                cmd.material = &resource_manager.get_material_instance(material_handle);
                cmd.transform = transform.get_model_matrix();
                cmd.view = view;
                cmd.projection = projection;

                this->render_queue_ptr->push(cmd);
            }
        );

        // Sort the rendering commands
        this->render_queue_ptr->sort();

        // Render the scene on the screen
        this->renderer_ptr->render(camera, *this->render_queue_ptr, resource_manager, editor_mode);

    }
};


#endif //ENGINE_RENDER_SYSTEM_HPP