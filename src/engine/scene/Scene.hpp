//
// Created by tomfr on 12/03/2026.
//

#ifndef ENGINE_SCENE_HPP
#define ENGINE_SCENE_HPP

#include <glm/glm.hpp>
#include "engine/resources/cpu/cpu_resource_manager.hpp"
#include "engine/ecs/registry.hpp"

class Scene{

    // Resource Manager class which load each resource to use
    CpuResourceManager& resource_manager;

    // ECS System which contains each object in the scene
    Registry registry;

public:
    explicit Scene(CpuResourceManager& resource_manager) : resource_manager(resource_manager){}

    Registry& get_registry()
    {
        return this->registry;
    }
    CpuResourceManager& get_resource_manager() const
    {
        return this->resource_manager;
    }
    template<typename T>
    void add_component(const entt::entity e, T component)
    {
        this->registry.add(e, component);
    }
    template<typename T>
    void remove_component(const entt::entity entity)
    {
        this->registry.remove<T>(entity);
    }

    entt::entity add_object()
    {
        return this->registry.create_entity();
    }
    void remove_object(const entt::entity entity)
    {
        this->registry.remove_entity(entity);
    }

    template<typename T>
    T& get_component(const entt::entity entity)
    {
        return this->registry.get<T>(entity);
    }
    entt::entity get_entity(glm::vec2 cursor_pos);
};

#endif //ENGINE_SCENE_HPP