//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_MATERIAL_INSTANCE_HPP
#define ENGINE_MATERIAL_INSTANCE_HPP

#include "material_resource.hpp"
#include "resource_handle.hpp"
#include <unordered_map>

class MaterialInstance
{
    ResourceHandle<MaterialInstance> material_resource;
    std::unordered_map<std::string, MaterialParameter> overrides;

public:
    explicit MaterialInstance(const ResourceHandle<MaterialInstance> material_resource) : material_resource(material_resource) {}
    ~MaterialInstance() = default;

    void set_override(const std::string& name, const MaterialParameter& parameter)
    {
        this->overrides[name] = parameter;
    }
    MaterialParameter* get_override(const std::string& name)
    {
        const auto it = this->overrides.find(name);
        if (it == this->overrides.end())
            return nullptr;

        return &it->second;
    }
    void delete_override(const std::string& name)
    {
        this->overrides.erase(name);
    }
};

#endif //ENGINE_MATERIAL_INSTANCE_HPP