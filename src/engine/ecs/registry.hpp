//
// Created by tomfr on 03/03/2026.
//

#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <cstdint>
#include "entity.hpp"
#include "entity_manager.hpp"
#include "component_storage.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <typeindex>

class Registry
{
    EntityManager entities;

    // Map within a component type id and an array of entity related to a component
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> pools;

public:
    Registry() = default;
    Entity create_entity()
    {
        return this->entities.create();
    }

    template<typename T>
    ComponentStorage<T>& storage()
    {
        const auto storage_index = std::type_index(typeid(T));
        if (!this->pools.contains(storage_index))
        {
            this->pools[storage_index] = std::make_unique<ComponentStorage<T>>();
        }

        return static_cast<ComponentStorage<T>&>(*this->pools[storage_index]);
    }

    template<typename T>
    void add(Entity entity, T component)
    {
        this->storage<T>().insert(entity, component);
    }
    template<typename T>
    void remove(Entity entity)
    {
        this->storage<T>().remove(entity);
    }
    template<typename T>
    T& get(Entity entity)
    {
        return this->storage<T>().get(entity);
    }

    template<typename First, typename... Rest>
    void view()
    {
        auto& first = storage<First>();

        for (auto& element : first.data())
        {
            Entity e = element.entity;

            if ((storage<Rest>().contains(e) && ...))
            {
                run_system(
                    first.get(e),
                    storage<Rest>().get(e)...
                );
            }
        }
    }

};




#endif //REGISTRY_HPP
