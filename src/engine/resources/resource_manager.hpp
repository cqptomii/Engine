//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_RESOURCE_MANAGER_HPP
#define ENGINE_RESOURCE_MANAGER_HPP

#include <string>
#include <unordered_map>
#include "resource_handle.hpp"
#include "shader_resource.hpp"
#include "material_instance.hpp"
#include "model_resource.hpp"

class ResourceManager
{
    // Cache CPU
    std::unordered_map<uint32_t, ModelResource> model_cache;
    std::unordered_map<uint32_t, MeshResource> mesh_cache;
    std::unordered_map<uint32_t, MaterialInstance> material_cache;
    std::unordered_map<uint32_t, TextureResource> texture_cache;
    std::unordered_map<uint32_t, ShaderResource> shader_cache;

    uint32_t next_model_id = 0;
    uint32_t next_material_instance_id = 0;
    uint32_t next_mesh_id = 0;
    uint32_t next_texture_id = 0;
    uint32_t next_shader_id = 0;
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    // Model
    ResourceHandle<ModelResource> load_model(const std::string& path);
    ModelResource& get_model(const ResourceHandle<ModelResource> handle)
    {
        return this->model_cache.at(handle.id);
    }

    // Mesh
    ResourceHandle<MeshResource> load_mesh(const std::string& path);
    MeshResource& get_mesh(const ResourceHandle<MeshResource> handle)
    {
        return this->mesh_cache.at(handle.id);
    }


    // Material
    ResourceHandle<MaterialInstance> load_material(const std::string& shader_path, const std::string& texture_path);
    MaterialInstance& get_material(const ResourceHandle<MaterialInstance> handle)
    {
        return this->material_cache.at(handle.id);
    }

    // Texture
    ResourceHandle<TextureResource> load_texture(const char* path);
    ResourceHandle<TextureResource> load_texture(const std::string& path)
    {
        return this->load_texture(path.c_str());
    }
    TextureResource& get_texture(const ResourceHandle<TextureResource> handle)
    {
        return this->texture_cache.at(handle.id);
    }
    // Shader
    ResourceHandle<ShaderResource> load_shader(const char* vertex_path, const char* fragment_path)
    {
        uint32_t id = next_shader_id++;
        this->shader_cache.emplace(id, ShaderResource(vertex_path, fragment_path));

        return ResourceHandle<ShaderResource>(id);
    }
    ResourceHandle<ShaderResource> load_shader(const std::string& vertex_path, const std::string& fragment_path)
    {
        return this->load_shader(vertex_path.c_str(), fragment_path.c_str());
    }
    ShaderResource& get_shader(const ResourceHandle<ShaderResource> handle)
    {
        return this->shader_cache.at(handle.id);
    }


};


#endif //ENGINE_RESOURCE_MANAGER_HPP