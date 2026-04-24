//
// Created by tomfr on 10/03/2026.
//

#ifndef ENGINE_MODEL_COMPONENT_HPP
#define ENGINE_MODEL_COMPONENT_HPP

#include "engine/resources/cpu/model/model_resource.hpp"
#include "engine/resources/cpu/resource_handle.hpp"

class ModelComponent
{
    ResourceHandle<ModelResource> model;
public:
    ModelComponent(ResourceHandle<ModelResource> model) : model(model) {}
    ResourceHandle<ModelResource> get_model() const
    {
        return this->model;
    }
};

#endif //ENGINE_MODEL_COMPONENT_HPP