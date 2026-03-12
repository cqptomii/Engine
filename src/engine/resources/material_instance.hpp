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
    ResourceHandle<MaterialResource> material_resource;
    std::unordered_map<uint32_t, MaterialParameter> overrides;

public:
    explicit MaterialInstance(const ResourceHandle<MaterialResource> material_resource) : material_resource(material_resource) {}
    ~MaterialInstance() = default;

    ResourceHandle<MaterialResource> get_material_resource() const
    {
        return this->material_resource;
    }

    void set_override(const uint32_t index, const MaterialParameter& parameter)
    {
        this->overrides[index] = parameter;
    }
    void set_override(const std::string& name, const MaterialParameter& parameter)
    {
        this->set_override(hash_string(name), parameter);
    }
    MaterialParameter* get_override(uint32_t id)
    {
        auto it = overrides.find(id);
        if (it != overrides.end())
            return &it->second;

        return nullptr;
    }
    MaterialParameter* get_override(const std::string& name)
    {
        return this->get_override(hash_string(name));
    }
    void delete_override(const uint32_t index)
    {
        this->overrides.erase(index);
    }
    void delete_override(const std::string& name)
    {
        this->delete_override(hash_string(name));
    }
    const std::unordered_map<uint32_t, MaterialParameter>& get_overrides() const
    {
        return this->overrides;
    }
};

#endif //ENGINE_MATERIAL_INSTANCE_HPP