#ifndef GPU_RESOURCE_MANAGER_HPP
#define GPU_RESOURCE_MANAGER_HPP

#include <unordered_map>
#include <memory>
#include <cstdint>

#include "engine/resources/gpu/wrappers/Buffer.hpp"
#include "engine/resources/cpu/resource_handle.hpp"
#include "engine/resources/cpu/mesh_resource.hpp"
#include "engine/resources/cpu/texture_resource.hpp"
#include "engine/resources/cpu/shader_resource.hpp"
#include "engine/resources/gpu/mesh.hpp"
#include "engine/resources/gpu/texture.hpp"
#include "engine/resources/gpu/shader.hpp"


class GPUResourceManager
{
private:
    // Caches for GPU resources
    std::unordered_map<uint32_t, std::unique_ptr<Mesh>> mesh_cache;
    std::unordered_map<uint32_t, std::unique_ptr<Texture>> texture_cache;
    std::unordered_map<uint32_t, std::unique_ptr<Shader>> shader_cache;

    // UBO related to the Camera data
    std::unique_ptr<Buffer> camera_ubo;
    std::unique_ptr<Buffer> object_ubo;
    std::unique_ptr<Buffer> material_ubo;

public:
    GPUResourceManager() = default;
    ~GPUResourceManager() = default;

    GPUResourceManager(const GPUResourceManager&) = delete;
    GPUResourceManager& operator=(const GPUResourceManager&) = delete;

    Mesh* get_mesh(const uint32_t id, const MeshResource& mesh_resource)
    {
        auto it = this->mesh_cache.find(id);
        if (it != this->mesh_cache.end())
        {
            return it->second.get();
        }

        const auto& vertices = mesh_resource.get_vertices();
        const auto& indices = mesh_resource.get_indices();

        auto mesh = std::make_unique<Mesh>(
            vertices.data(),
            vertices.size(),
            indices.data(),
            indices.size()
        );

        Mesh* mesh_ptr = mesh.get();
        this->mesh_cache.emplace(id, std::move(mesh));
        return mesh_ptr;
    }
    Mesh* get_mesh(const ResourceHandle<MeshResource> handle, const MeshResource& mesh_resource)
    {
        return this->get_mesh(handle.get_id(), mesh_resource);
    }
    
    Texture* get_texture(const uint32_t id, const TextureResource& texture_resource)
    {
        auto it = this->texture_cache.find(id);
        if (it != this->texture_cache.end())
        {
            return it->second.get();
        }

        auto texture = std::make_unique<Texture>(
            texture_resource.get_data(),
            texture_resource.get_width(),
            texture_resource.get_height(),
            texture_resource.get_nr_channels()
        );

        Texture* texture_ptr = texture.get();
        this->texture_cache.emplace(id, std::move(texture));
        return texture_ptr;
    }
    Texture* get_texture(const ResourceHandle<TextureResource> handle, const TextureResource& texture_resource)
    {
        return this->get_texture(handle.get_id(), texture_resource);
    }

    Shader* get_shader(const uint32_t id, const ShaderResource& shader_resource)
    {
        auto it = this->shader_cache.find(id);
        if (it != this->shader_cache.end())
        {
            return it->second.get();
        }

        auto shader = std::make_unique<Shader>(
            shader_resource.get_vertex_source().c_str(),
            shader_resource.get_fragment_source().c_str()
        );

        Shader* shader_ptr = shader.get();
        this->shader_cache.emplace(id, std::move(shader));
        return shader_ptr;
    }
    Shader* get_shader(const ResourceHandle<ShaderResource> handle, const ShaderResource& shader_resource)
    {
        return this->get_shader(handle.get_id(), shader_resource);
    }


    void set_camera_ubo_data(const void* data, size_t size)
    {
        if (!this->camera_ubo)
        {
            this->camera_ubo = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
            this->camera_ubo->bind();
            this->camera_ubo->set_data(size, data, GL_DYNAMIC_DRAW);
        }
        else
        {
            this->camera_ubo->bind();
            this->camera_ubo->update_data(0, size, data);
        }

        this->camera_ubo->bind_base(0);
    }
    void set_object_ubo_data(const void* data, size_t size)
    {
        if (!this->object_ubo)
        {
            this->object_ubo = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
            this->object_ubo->bind();
            this->object_ubo->set_data(size, data, GL_DYNAMIC_DRAW);
        }
        else
        {
            this->object_ubo->bind();
            this->object_ubo->update_data(0, size, data);
        }

         this->object_ubo->bind_base(1);
    }
    void set_material_ubo_data(const void* data, size_t size)
    {
        if (!this->material_ubo)
        {
            this->material_ubo = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
            this->material_ubo->bind();
            this->material_ubo->set_data(size, data, GL_DYNAMIC_DRAW);
        }
        else
        {
            this->material_ubo->bind();
            this->material_ubo->update_data(0, size, data);
        }
    }

    Buffer* get_camera_ubo()
    {
        if (!this->camera_ubo)
        {
            this->camera_ubo = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
        }
        return this->camera_ubo.get();
    }
    Buffer* get_object_ubo()
    {
        if (!this->object_ubo)
        {
            this->object_ubo = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
        }
        return this->object_ubo.get();
    }
    Buffer* get_material_ubo()
    {
        if (!this->material_ubo)
        {
            this->material_ubo = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
        }
        return this->material_ubo.get();
    }

    std::size_t count_meshes() const {
        return this->mesh_cache.size();
    }

    std::size_t count_textures() const {
        return this->texture_cache.size();
    }

    std::size_t count_shaders() const {
        return this->shader_cache.size();
    }

    std::size_t count_ubos() const {
        std::size_t count = 0;
        if (this->camera_ubo) {
            ++count;
        }
        if (this->object_ubo) {
            ++count;
        }
        if (this->material_ubo) {
            ++count;
        }
        return count;
    }
};



#endif // GPU_RESOURCE_MANAGER_HPP