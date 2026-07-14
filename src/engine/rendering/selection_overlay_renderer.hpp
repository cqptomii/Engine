#ifndef SELECTION_OVERLAY_RENDERER_HPP
#define SELECTION_OVERLAY_RENDERER_HPP

#include <vector>

#include <glad/glad.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "engine/core/utils.hpp"
#include "engine/resources/cpu/mesh_resource.hpp"
#include "engine/resources/gpu/mesh.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/ecs/components/mesh_component.hpp"

#include "engine/rendering/utils/camera_data.hpp"
#include "engine/resources/cpu/cpu_resource_manager.hpp"
#include "engine/resources/cpu/shader_resource.hpp"
#include "engine/resources/gpu/gpu_resource_manager.hpp"
#include "engine/resources/gpu/shader.hpp"
#include "engine/scene/Scene.hpp"
#include "engine/scene/scene_hierarchy.hpp"

class SelectionOverlayRenderer
{
    std::string color_vs_path;
    std::string color_fs_path;

public:
    SelectionOverlayRenderer()
    {
        color_vs_path = resolve_shader_path("assets/shaders/color.vs");
        color_fs_path = resolve_shader_path("assets/shaders/color.fs");
    }

    ~SelectionOverlayRenderer() = default;

    void render(
        const CameraData& camera_data,
        Scene& scene,
        const std::vector<entt::entity>& selected_entities,
        GPUResourceManager& gpu_resource_manager,
        CpuResourceManager& cpu_resource_manager) const
    {
        if (selected_entities.empty())
        {
            return;
        }

        static ShaderResource shader_resource(color_vs_path.c_str(), color_fs_path.c_str());
        static Shader shader(
            shader_resource.get_vertex_source().c_str(),
            shader_resource.get_fragment_source().c_str()
        );

        GLint polygon_mode[2] = {GL_FILL, GL_FILL};
        glGetIntegerv(GL_POLYGON_MODE, polygon_mode);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2.0f);

        shader.use();
        shader.set_mat4("view", camera_data.view);
        shader.set_mat4("projection", camera_data.projection);
        shader.set_vec3("lightColor", glm::vec3(1.0f));
        shader.set_vec4("objectColor", glm::vec4(1.0f, 0.55f, 0.0f, 1.0f));

        Registry& registry = scene.get_registry();

        for (const entt::entity entity : selected_entities)
        {
            if (!registry.raw().valid(entity))
            {
                continue;
            }

            if (!registry.raw().all_of<TransformComponent, MeshComponent>(entity))
            {
                continue;
            }

            const TransformComponent& transform = registry.get<TransformComponent>(entity);
            const MeshComponent& mesh_component = registry.get<MeshComponent>(entity);
            const ResourceHandle<MeshResource> mesh_handle = mesh_component.get_mesh();
            if (!mesh_handle)
            {
                continue;
            }

            MeshResource& mesh_resource = cpu_resource_manager.get_mesh(mesh_handle);
            Mesh* mesh = gpu_resource_manager.get_mesh(mesh_handle, mesh_resource);
            if (mesh == nullptr)
            {
                continue;
            }

            shader.set_mat4("model", scene_hierarchy::get_world_matrix(scene, entity));
            mesh->draw();
        }

        glPolygonMode(GL_FRONT_AND_BACK, polygon_mode[1]);
    }
};

#endif // SELECTION_OVERLAY_RENDERER_HPP
