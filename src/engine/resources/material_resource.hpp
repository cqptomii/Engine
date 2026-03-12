//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_MATERIAL_RESOURCE_HPP
#define ENGINE_MATERIAL_RESOURCE_HPP

#include "resource_handle.hpp"
#include "shader_resource.hpp"
#include "texture_resource.hpp"
#include "../src/engine/utils.hpp"
#include <unordered_map>
#include <variant>
#include <glm/glm.hpp>

enum class MaterialParameterType
{
    FLOAT,
    INT,
    VEC2,
    VEC3,
    VEC4,
    MAT4,
    TEXTURE
};

struct MaterialParameter
{
    std::variant<float, int, glm::vec2, glm::vec3, glm::vec4, glm::mat4, ResourceHandle<TextureResource>> value;
};


class MaterialResource
{
    ResourceHandle<ShaderResource> shader;
    std::unordered_map<uint32_t, MaterialParameter> parameters;

public:
    explicit MaterialResource(const ResourceHandle<ShaderResource> shader) : shader(shader){}
    ~MaterialResource() = default;

    ResourceHandle<ShaderResource> get_shader() const
    {
        return this->shader;
    }
    void set_parameter(const uint32_t index, const MaterialParameter& parameter)
    {
        this->parameters[index] = parameter;
    }
    void set_parameter(const std::string& name, const MaterialParameter& parameter)
    {
        this->parameters[hash_string(name)] = parameter;
    }
    MaterialParameter* get_parameter(const uint32_t index)
    {
        const auto it = this->parameters.find(index);
        if (it != this->parameters.end())
        {
            return &it->second;
        }
        return nullptr;
    }
    MaterialParameter* get_parameter(const std::string& name)
    {
        return get_parameter(hash_string(name));
    }
    void delete_parameter(const uint32_t index)
    {
        this->parameters.erase(index);
    }
    void delete_parameter(const std::string& name)
    {
        delete_parameter(hash_string(name));
    }
    const std::unordered_map<uint32_t, MaterialParameter>& get_parameters() const
    {
        return this->parameters;
    }
};

#endif //ENGINE_MATERIAL_RESOURCE_HPP