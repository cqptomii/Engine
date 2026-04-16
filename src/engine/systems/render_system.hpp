//
// Created by tomfr on 10/03/2026.
//

#ifndef ENGINE_RENDER_SYSTEM_HPP
#define ENGINE_RENDER_SYSTEM_HPP

#include "engine/scene/Scene.hpp"
#include "engine/rendering/camera_data.hpp"
#include "engine/rendering/render_queue.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/ecs/components/model_component.hpp"
#include "engine/ecs/components/material_component.hpp"


class RenderSystem
{
public:
    explicit RenderSystem() = default;
    void update(Scene& scene, const CameraData& camera, RenderQueue& queue, const bool is_editor_mode)
    {
        // Set the matrix given by the main camera of the scene
        glm::mat4 view, projection;

        if (is_editor_mode)
        {
            view = camera.view;
            projection = camera.projection;
        }else
        {
            // Get the main camera into the scene

        }

        auto& registry = scene.get_registry();
        auto& resource_manager = scene.get_resource_manager();

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
                        cmd.mesh = &mesh;
                        cmd.material = &resource_manager.get_material_instance(selected_material);

                        // Set Transformation Matrix to get the relative position of the model to the main camera
                        cmd.transform = transform.get_model_matrix();
                        cmd.view = view;
                        cmd.projection = projection;

                        queue.push(cmd);
                    }
                }
            }
        );

        /***
        // Update Shaders
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view_matrix[0][0]);


        // Rendering
        glBindVertexArray(VAO);
        for (auto cubePosition : cubePositions)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            auto model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePosition);

            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        **/
    }
};


#endif //ENGINE_RENDER_SYSTEM_HPP