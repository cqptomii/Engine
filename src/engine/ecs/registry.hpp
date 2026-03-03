//
// Created by tomfr on 03/03/2026.
//

#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <cstdint>
#include "entity.hpp"
#include "component_storage.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <typeindex>

class Registry
{
    std::uint32_t next_entity_id;

    // Map within a component type id and an array of entity related to a component
    std::unordered_map<std::uint32_t, std::unique_ptr<IComponentStorage>> componentsArrays;

    template<typename T>
    ComponentStorage<T> get_array()
    {
        const auto component_index = std::type_index(typeid(T));
        assert(this->componentsArrays.contains(component_index.hash_code()));
        return static_cast<ComponentStorage<T>*>(this->componentsArrays[component_index.hash_code()].get());
    }

    template<typename T>
    bool has_component(Entity e)
    {
        const auto component_index = std::type_index(typeid(T));

        if (!this->componentsArrays.contains(component_index.hash_code()))
        {
            return false;
        }

        auto array = static_cast<ComponentStorage<T>*>(this->componentsArrays[component_index.hash_code()].get());

        return array->contains(e);
    }




public:
    Registry() : next_entity_id(1) {}

    Entity create_entity()
    {
        return {this->next_entity_id++, 0};
    }

    template<typename T>
    void addComponent(Entity entity, T component)
    {
        const auto component_index = std::type_index(typeid(T));
        if (!this->componentsArrays.contains(component_index.hash_code()))
        {
            this->componentsArrays[component_index.hash_code()] = std::make_unique<ComponentStorage<T>>();
        }

        auto array = static_cast<ComponentStorage<T>*>(this->componentsArrays[component_index.hash_code()].get());
        array->insert(entity, component);
    }

    template<typename T>
    void removeComponent(Entity entity)
    {
        const auto component_index = std::type_index(typeid(T));
        if (this->componentsArrays.contains(component_index.hash_code()))
        {
            auto array = static_cast<ComponentStorage<T>*>(this->componentsArrays[component_index.hash_code()].get());
            array->remove(entity);
        }
    }

    template<typename T>
    T& getComponent(Entity entity)
    {
        const auto component_index = std::type_index(typeid(T));
        auto array = static_cast<ComponentStorage<T>*>(this->componentsArrays[component_index.hash_code()].get());
        return array->get(entity);
    }

    template<typename ...T>
    std::vector<Entity> view()
    {
        std::vector<Entity> entities;

        if constexpr (sizeof...(T) == 0) return entities;


        ComponentStorage<T> array = this->get_array<T>();
        for (const auto& [id, generation] : array.get_data())
        {
            Entity e = {id, generation};
            if ((this->has_component<T>(e) && ...)) {
                entities.push_back(e);
            }
        }


        return entities;
    }

};




#endif //REGISTRY_HPP
