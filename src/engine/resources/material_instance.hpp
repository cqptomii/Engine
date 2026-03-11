//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_MATERIAL_INSTANCE_HPP
#define ENGINE_MATERIAL_INSTANCE_HPP

#include "material_resource.hpp"
#include <unordered_map>

class MaterialInstance
{
    MaterialResource* material_resource;
    std::unordered_map<std::string, ResourceHandle<MaterialParameter>> overrides;

public:
    explicit MaterialInstance(MaterialResource* material_resource) : material_resource(material_resource) {}
    ~MaterialInstance()
    {
        delete material_resource;
    };

    void set_override(const std::string& name, const ResourceHandle<MaterialParameter>& parameter)
    {
        this->overrides[name] = parameter;
    }
    ResourceHandle<MaterialParameter> get_override(const std::string& name)
    {
        return this->overrides[name];
    }
    void delete_override(const std::string& name)
    {
        this->overrides.erase(name);
    }
};

#endif //ENGINE_MATERIAL_INSTANCE_HPP