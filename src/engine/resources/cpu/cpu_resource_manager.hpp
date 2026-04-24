//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_RESOURCE_MANAGER_HPP
#define ENGINE_RESOURCE_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include "engine/resources/cpu/resource_handle.hpp"
#include "engine/resources/cpu/shader_resource.hpp"
#include "engine/resources/cpu/material/material_resource.hpp"
#include "engine/resources/cpu/material/material_instance.hpp"
#include "engine/resources/cpu/model/model_resource.hpp"
#include "engine/resources/cpu/mesh_resource.hpp"
#include "engine/utils.hpp"

class CpuResourceManager
{
    // Cache CPU
    std::unordered_map<uint32_t, std::unique_ptr<ModelResource>> model_cache;
    std::unordered_map<uint32_t, std::unique_ptr<MeshResource>> mesh_cache;
    std::unordered_map<uint32_t, std::unique_ptr<MaterialResource>> material_cache;
    std::unordered_map<uint32_t, std::unique_ptr<MaterialInstance>> material_instance_cache;
    std::unordered_map<uint32_t, std::unique_ptr<TextureResource>> texture_cache;
    std::unordered_map<uint32_t, std::unique_ptr<ShaderResource>> shader_cache;

    uint32_t next_material_instance_id = 0;

    // hashed path - id mapping
    std::unordered_map<uint32_t, std::string> id_to_path_cache;

    void throw_hash_collision(const std::string& path, const std::string& existing_path)
    {
        throw std::runtime_error("Hash collision detected for resource: \n" + path + "\n" + existing_path + "\nConsider renaming one of the resources to avoid the collision.");
    }

public:
    CpuResourceManager() = default;
    ~CpuResourceManager() = default;

    CpuResourceManager(const CpuResourceManager&) = delete;
    CpuResourceManager& operator=(const CpuResourceManager&) = delete;

    // Model
    ResourceHandle<ModelResource> load_model(const std::string& path){
        uint32_t id = hash_string(
            normalize_resource_path_minimal(path)
        );
        if (this->model_cache.find(id) == this->model_cache.end())
        {
            this->model_cache.emplace(id, std::make_unique<ModelResource>(path, *this));
        }
        return ResourceHandle<ModelResource>(id);
    }
    ModelResource& get_model(const ResourceHandle<ModelResource> handle)
    {
        return *this->model_cache.at(handle.id);
    }

    // Mesh
    ResourceHandle<MeshResource> load_mesh(const std::string& path, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        uint32_t id = hash_string(
            normalize_resource_path_minimal(path)
        );
        if( this->mesh_cache.find(id) == this->mesh_cache.end())
        {
             this->mesh_cache.emplace(id, std::make_unique<MeshResource>(vertices, indices));
             this->id_to_path_cache.emplace(id, path);
        }else{
            // Check for hash collision
            const std::string& existing_path = this->id_to_path_cache.at(id);
            if (existing_path != path)
            {
                this->throw_hash_collision(path, existing_path);
            }
        }
        return ResourceHandle<MeshResource>(id);
    }
    MeshResource& get_mesh(const ResourceHandle<MeshResource> handle)
    {
        return *this->mesh_cache.at(handle.id);
    }

    // Material
    ResourceHandle<MaterialResource> load_material_resource(const std::string& path, const char* vertex_shader_path, const char* fragment_shader_path, const std::vector<std::string> texture_paths)
    {
        uint32_t id = hash_string(
            normalize_resource_path_minimal(path)
        );

        if (this->material_cache.find(id) == this->material_cache.end())
        {

            // Load the shader associated with the material
            const ResourceHandle<ShaderResource> mat_shader = this->load_shader(path + "::shader", vertex_shader_path, fragment_shader_path);
            
            // Load the textures associated with the material
            std::unordered_map<std::string, ResourceHandle<TextureResource>> textures;
            uint32_t texture_index = 0;
            for (const auto& path : texture_paths)
            {
                textures.emplace("texture_" + std::to_string(texture_index++), this->load_texture(path));
            }

            // Add the material in the cache
            this->material_cache.emplace(id, std::make_unique<MaterialResource>(mat_shader, textures));
            
            this->id_to_path_cache.emplace(id, path);
        }else{
            // Check for hash collision
            const std::string& existing_path = this->id_to_path_cache.at(id);
            if (existing_path != path)
            {
                this->throw_hash_collision(path, existing_path);
            }
        }
        return ResourceHandle<MaterialResource>(id);
    }
    MaterialResource& get_material_resource(const ResourceHandle<MaterialResource> handle)
    {
        return *this->material_cache.at(handle.id);
    }
    ResourceHandle<MaterialInstance> create_material_instance(const ResourceHandle<MaterialResource> material_resource)
    {
        uint32_t id = next_material_instance_id++;
        this->material_instance_cache.emplace(id, std::make_unique<MaterialInstance>(material_resource));
        return ResourceHandle<MaterialInstance>(id);
    }
    MaterialInstance& get_material_instance(const ResourceHandle<MaterialInstance> handle)
    {
        return *this->material_instance_cache.at(handle.id);
    }
    
    // Texture
    ResourceHandle<TextureResource> load_texture(const char* path)
    {
        uint32_t id = hash_string(
            normalize_resource_path_minimal(path)
        );
        if (this->texture_cache.find(id) == this->texture_cache.end())
        {
            // Load the texture from path
            this->texture_cache.emplace(id, std::make_unique<TextureResource>(path));

            this->id_to_path_cache.emplace(id, path);
        }else{
            // Check for hash collision
            const std::string& existing_path = this->id_to_path_cache.at(id);
            if (existing_path != path)
            {
                this->throw_hash_collision(path, existing_path);
            }
        }
        return ResourceHandle<TextureResource>(id);
    }
    ResourceHandle<TextureResource> load_texture(const std::string& path)
    {
        return this->load_texture(path.c_str());
    }
    TextureResource& get_texture(const ResourceHandle<TextureResource> handle)
    {
        return *this->texture_cache.at(handle.id);
    }
    
    // Shader
    ResourceHandle<ShaderResource> load_shader(const std::string& path, const char* vertex_path, const char* fragment_path)
    {
        uint32_t id = hash_string(
            normalize_resource_path_minimal(path)
        );
        if (this->shader_cache.find(id) == this->shader_cache.end())
        {
            this->shader_cache.emplace(id, std::make_unique<ShaderResource>(vertex_path, fragment_path));
            this->id_to_path_cache.emplace(id, path);
        }else{
            // Check for hash collision
            const std::string& existing_path = this->id_to_path_cache.at(id);
            if (existing_path != path)
            {
                this->throw_hash_collision(path, existing_path);
            }
        }
        return ResourceHandle<ShaderResource>(id);
    }
    ResourceHandle<ShaderResource> load_shader(const std::string& path, const std::string& vertex_path, const std::string& fragment_path)
    {
        return this->load_shader(path, vertex_path.c_str(), fragment_path.c_str());
    }
    ShaderResource& get_shader(const ResourceHandle<ShaderResource> handle)
    {
        return *this->shader_cache.at(handle.id);
    }
};

#endif //ENGINE_RESOURCE_MANAGER_HPP