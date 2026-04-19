//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_RENDERER_HPP
#define ENGINE_RENDERER_HPP

#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include  "mesh.hpp"
#include "render_queue.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "engine/core/wrapper/Buffer.hpp"
#include "ubo_types.hpp"
#include "camera_data.hpp"
#include "../resources/material/material_resource.hpp"
#include "GizmoRenderer.hpp"
#include "GridRenderer.hpp"
#include "TransformData.hpp"

template<typename>
struct always_false : std::false_type {};

class Renderer
{
    
    std::unordered_map<MeshResource*, Mesh*> mesh_cache;
    std::unordered_map<TextureResource*, Texture*> texture_cache;
    std::unordered_map<ShaderResource*, Shader*> shader_cache;

    // UBO related to the Camera data
    std::unique_ptr<Buffer> camera_ubo;
    std::unique_ptr<Buffer> object_ubo;
    std::unique_ptr<Buffer> material_ubo;

    // Rendering related classes related to the editor gizmo and the editor grid
    std::unique_ptr<GizmoRenderer> gizmo_renderer;
    std::unique_ptr<GridRenderer> grid_renderer;

    void ensure_gpu_buffers_initialized()
    {
        if (!this->camera_ubo)
        {
            this->camera_ubo = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
            this->camera_ubo->setData(sizeof(CameraUBO), nullptr, GL_DYNAMIC_DRAW);
        }
        if (!this->object_ubo)
        {
            this->object_ubo = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
            this->object_ubo->setData(sizeof(ObjectUBO), nullptr, GL_DYNAMIC_DRAW);
        }
        if (!this->material_ubo)
        {
            this->material_ubo = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
            this->material_ubo->setData(sizeof(MaterialUBO), nullptr, GL_DYNAMIC_DRAW);
        }
    }

public:

    Renderer() = default;
    ~Renderer() = default;

    Mesh* get_mesh(MeshResource& mesh_resource)
    {
        if (!this->mesh_cache.contains(&mesh_resource))
        {
            const auto mesh = new Mesh(mesh_resource);
            this->mesh_cache[&mesh_resource] = mesh;
            return mesh;
        }

        return this->mesh_cache[&mesh_resource];
    }
    Texture* get_texture(TextureResource& texture_resource)
    {
        if (!this->texture_cache.contains(&texture_resource))
        {
            const auto texture = new Texture(texture_resource);
            this->texture_cache[&texture_resource] = texture;
            return texture;
        }

        return this->texture_cache[&texture_resource];
    }
    Shader* get_shader(ShaderResource& shader_resource)
    {
        if (!this->shader_cache.contains(&shader_resource))
        {
            const auto shader = new Shader(shader_resource);
            this->shader_cache[&shader_resource] = shader;
            return shader;
        }

        return this->shader_cache[&shader_resource];
    }

    void execute(RenderQueue& queue, ResourceManager& resource_manager)
    {
        this->ensure_gpu_buffers_initialized();

        for (auto& cmd : queue.get_commands())
        {
            if (cmd.mesh == nullptr || cmd.material == nullptr)
            {
                continue;
            }

            Mesh* mesh = this->get_mesh(*cmd.mesh);


            // Load the Material
            MaterialInstance* material_instance = cmd.material;
            MaterialResource& material_resource = resource_manager.get_material_resource(material_instance->get_material_resource());

            // Load the given shader
            ShaderResource&  shader_resource= resource_manager.get_shader(material_resource.get_shader());
            Shader* shader = this->get_shader(shader_resource);

            // Update Shader Uniform related to the Camera Transformation
            const CameraUBO cam_data{cmd.view, cmd.projection};
            this->camera_ubo->updateData(0, sizeof(CameraUBO), &cam_data);
           
            // Update Shader Uniform related to the Model Transformation
            const ObjectUBO model_data{cmd.transform};
            this->object_ubo->updateData(0, sizeof(ObjectUBO), &model_data);

            // Get each parameter of the material to update
            auto parameters = material_resource.get_parameters();
            auto override_parameter = material_instance->get_overrides();
            for (const auto id : parameters | std::views::keys)
            {
                if (override_parameter.contains(id))
                {
                    parameters[id] = override_parameter[id];
                }
            }

            // Use the Shader
            shader->use();

            int texture_slot = 0;
            // Update Shader Uniform related to the Material parameters and texture bindings
            for (const auto& parameter : parameters | std::views::values)
            {
                this->apply_parameter(
                    *shader,
                    parameter,
                    texture_slot,
                    resource_manager
                );

                texture_slot++;
            }

            // Draw the mesh on the viewport
            mesh->draw();
        }
    }
    void apply_parameter(Shader& shader, const MaterialParameter& parameter,int texture_slot, ResourceManager& resource_manager)
    {

        std::visit([&](auto&& value)
        {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, ResourceHandle<TextureResource>>)
            {
                const Texture* texture = this->get_texture(resource_manager.get_texture(value));
                texture->bind(texture_slot);
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                shader.set_float(parameter.name, value);
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                shader.set_int(parameter.name, value);
            }
            else if constexpr (std::is_same_v<T, glm::vec2>)
            {
                shader.set_vec2(parameter.name, value);
            }
            else if constexpr (std::is_same_v<T, glm::vec3>)
            {
                shader.set_vec3(parameter.name, value);
            }
            else if constexpr (std::is_same_v<T, glm::vec4>)
            {
                shader.set_vec4(parameter.name, value);
            }
            else if constexpr (std::is_same_v<T, glm::mat4>)
            {
                shader.set_mat4(parameter.name, value);
            }
            else
            {
                static_assert(always_false<T>::value, "Unhandled material parameter type");
            }

        }, parameter.value);

    }

    void render(const CameraData& camera_data, RenderQueue& queue, ResourceManager& resource_manager, bool render_editor_elements = false)
    {
        // Render the scene onto the screen
        this->execute(queue, resource_manager);


        // If we are in editor mode, we render the editor grid and the editor gizmo on top right of the viewport
        if (render_editor_elements)
        {
            if (!this->gizmo_renderer)
            {
                // Create GPU gizmo resources only after a valid OpenGL context exists.
                this->gizmo_renderer = std::make_unique<GizmoRenderer>();
            }
            if (!this->grid_renderer)
            {
                // Create GPU grid resources only after a valid OpenGL context exists.
                this->grid_renderer = std::make_unique<GridRenderer>();
            }

            // Render the editor grid 
            this->grid_renderer->render(camera_data);
            
            const glm::mat4 gizmo_model = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f));
            const glm::mat4 gizmo_view = glm::mat4(glm::mat3(camera_data.view));
            const glm::mat4 gizmo_projection = glm::ortho(-1.2f, 1.2f, -1.2f, 1.2f, -2.0f, 2.0f);

            // Upload the gizmo data and render the editor gizmo
            std::vector<TransformData> gizmo_transforms{
                {gizmo_model, gizmo_view, gizmo_projection}
            };
            this->gizmo_renderer->upload_gizmo(gizmo_transforms);
            this->gizmo_renderer->render(camera_data);
        }
    }
};

#endif //ENGINE_RENDERER_HPP