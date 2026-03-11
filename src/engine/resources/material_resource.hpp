//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_MATERIAL_RESOURCE_HPP
#define ENGINE_MATERIAL_RESOURCE_HPP

#include "resource_handle.hpp"
#include "shader_resource.hpp"
#include "texture_resource.hpp"
#include <unordered_map>
#include <variant>
#include <glm/glm.hpp>

struct MaterialParameter
{
    std::variant<float, glm::vec2, glm::vec3, ResourceHandle<TextureResource>> value;
};


class MaterialResource
{
    ResourceHandle<ShaderResource> shader;
    std::unordered_map<std::string, MaterialParameter> parameters;

public:
    explicit MaterialResource(const ResourceHandle<ShaderResource> shader) : shader(shader) {}
    ~MaterialResource() = default;

    void set_parameter(const std::string& name, const MaterialParameter& parameter)
    {
        this->parameters[name] = parameter;
    }
    MaterialParameter get_parameter(const std::string& name)
    {
        return this->parameters[name];
    }
    void delete_parameter(const std::string& name)
    {
        this->parameters.erase(name);
    }
};

#endif //ENGINE_MATERIAL_RESOURCE_HPP