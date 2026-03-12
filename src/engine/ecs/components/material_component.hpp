//
// Created by tomfr on 12/03/2026.
//

#ifndef ENGINE_MATERIAL_COMPONENT_HPP
#define ENGINE_MATERIAL_COMPONENT_HPP

#include "../src/engine/resources/resource_handle.hpp"
#include "../src/engine/resources/material_instance.hpp"

class MaterialComponent
{
    ResourceHandle<MaterialInstance> material;
public:
    MaterialComponent(const ResourceHandle<MaterialInstance> material) : material(material) {}
    ResourceHandle<MaterialInstance> get_material() const
    {
        return this->material;
    }
};

#endif //ENGINE_MATERIAL_COMPONENT_HPP