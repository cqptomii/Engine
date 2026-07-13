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

#include "engine/rendering/render_queue.hpp"
#include "engine/rendering/utils/ubo_types.hpp"
#include "engine/rendering/utils/camera_data.hpp"
#include "engine/rendering/utils/transform_data.hpp"
#include "engine/rendering/gizmo/gizmo_renderer.hpp"
#include "engine/rendering/gizmo/transform_gizmo_renderer.hpp"
#include "engine/rendering/grid_renderer.hpp"
#include "engine/rendering/selection_overlay_renderer.hpp"

#include "engine/editor/picking/manipulation_mode.hpp"
#include "engine/editor/picking/selection_context.hpp"

#include <entt/entt.hpp>
#include "engine/scene/Scene.hpp"

#include "engine/resources/gpu/wrappers/Buffer.hpp"

#include "engine/resources/cpu/material/material_resource.hpp"
#include "engine/resources/cpu/cpu_resource_manager.hpp"
#include "engine/resources/gpu/mesh.hpp"
#include "engine/resources/gpu/shader.hpp"
#include "engine/resources/gpu/texture.hpp"
#include "engine/resources/gpu/gpu_resource_manager.hpp"


template<typename>
struct always_false : std::false_type {};

class Renderer
{
    
    // GPU Resource Manager
    std::unique_ptr<GPUResourceManager> gpu_resource_manager;

    // Rendering related classes related to the editor gizmo and the editor grid
    std::unique_ptr<GizmoRenderer> gizmo_renderer;
    std::unique_ptr<GridRenderer> grid_renderer;
    std::unique_ptr<SelectionOverlayRenderer> selection_overlay_renderer;
    std::unique_ptr<TransformGizmoRenderer> transform_gizmo_renderer;

public:

    Renderer(){
        this->gpu_resource_manager = std::make_unique<GPUResourceManager>();
    };
    ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void execute(RenderQueue& queue, CpuResourceManager& resource_manager)
    {
        for (auto& cmd : queue.get_commands())
        {
            if (!cmd.mesh_handle || cmd.mesh == nullptr || cmd.material == nullptr)
            {
                continue;
            }

            Mesh* mesh = this->gpu_resource_manager->get_mesh(cmd.mesh_handle, *cmd.mesh);

            // Load the Material
            MaterialInstance* material_instance = cmd.material;
            MaterialResource& material_resource = resource_manager.get_material_resource(material_instance->get_material_resource());

            // Load the given shader
            ShaderResource&  shader_resource= resource_manager.get_shader(material_resource.get_shader());
            Shader* shader = this->gpu_resource_manager->get_shader(material_resource.get_shader(), shader_resource);

            shader->bind_ubo("CameraData", 0);
            shader->bind_ubo("ModelData", 1);

            // Update Shader Uniform related to the Camera Transformation
            const CameraUBO cam_data{cmd.view, cmd.projection};
            this->gpu_resource_manager->set_camera_ubo_data(&cam_data, sizeof(CameraUBO));
           
            // Update Shader Uniform related to the Model Transformation
            const ObjectUBO model_data{cmd.transform};
            this->gpu_resource_manager->set_object_ubo_data(&model_data, sizeof(ObjectUBO));

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
    void apply_parameter(Shader& shader, const MaterialParameter& parameter,int texture_slot, CpuResourceManager& resource_manager)
    {

        std::visit([&](auto&& value)
        {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, ResourceHandle<TextureResource>>)
            {
                const Texture* texture = this->gpu_resource_manager->get_texture(value, resource_manager.get_texture(value));
                
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

    void render(
        const CameraData& camera_data,
        Scene& scene,
        RenderQueue& queue,
        CpuResourceManager& resource_manager,
        bool render_editor_elements = false,
        const std::vector<entt::entity>& selected_entities = {},
        ManipulationMode manipulation_mode = ManipulationMode::NONE)
    {
        // Render the scene onto the screen
        this->execute(queue, resource_manager);

        if (render_editor_elements && !selected_entities.empty())
        {
            if (!this->selection_overlay_renderer)
            {
                this->selection_overlay_renderer = std::make_unique<SelectionOverlayRenderer>();
            }

            this->selection_overlay_renderer->render(
                camera_data,
                scene,
                selected_entities,
                *this->gpu_resource_manager,
                resource_manager
            );

            // Draw the transform gizmo (translate/rotate/scale) at the selection pivot.
            if (manipulation_mode != ManipulationMode::NONE)
            {
                if (!this->transform_gizmo_renderer)
                {
                    this->transform_gizmo_renderer = std::make_unique<TransformGizmoRenderer>();
                }

                const SelectionContext selection_context = build_selection_context(scene, selected_entities);
                this->transform_gizmo_renderer->render(manipulation_mode, selection_context, camera_data);
            }
        }

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

    GPUResourceManager& get_gpu_resource_manager() {
        return *this->gpu_resource_manager;
    }
};

#endif //ENGINE_RENDERER_HPP