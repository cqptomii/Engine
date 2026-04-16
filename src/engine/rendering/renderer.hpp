//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_RENDERER_HPP
#define ENGINE_RENDERER_HPP

#include <type_traits>
#include <unordered_map>
#include  "mesh.hpp"
#include "render_queue.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "../resources/material/material_resource.hpp"
#include "../src/engine/resources/material/UniformBuffer.hpp"

template<typename>
struct always_false : std::false_type {};

class Renderer
{
    std::unordered_map<MeshResource*, Mesh*> mesh_cache;
    std::unordered_map<TextureResource*, Texture*> texture_cache;
    std::unordered_map<ShaderResource*, Shader*> shader_cache;

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

            auto camBuffer = UniformBuffer(sizeof(CameraUBO), 0);
            auto modelBuffer = UniformBuffer(sizeof(ObjectUBO), 1);
            auto materialBuffer = UniformBuffer(sizeof(MaterialUBO), 2);

            // Update Shader Uniform related to the Camera Transformation
            CameraUBO cam_data{cmd.view, cmd.projection};
            camBuffer.update(&cam_data);
            // Update Shader Uniform related to the Model Transformation
            ObjectUBO model_data{cmd.transform};
            modelBuffer.update(&model_data);


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

            // Use the Shader
            shader->use();

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
};

#endif //ENGINE_RENDERER_HPP