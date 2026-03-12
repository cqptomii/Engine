//
// Created by tomfr on 10/03/2026.
//

#ifndef ENGINE_MODEL_COMPONENT_HPP
#define ENGINE_MODEL_COMPONENT_HPP

#include "../../resources/model_resource.hpp"
#include "../../resources/resource_handle.hpp"

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