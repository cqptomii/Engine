//
// Created by tomfr on 03/03/2026.
//

#ifndef COMPONENT_STORAGE_HPP
#define COMPONENT_STORAGE_HPP

#include "./entity.hpp"
#include <unordered_map>
#include <memory>
#include <cstdint>

#include "glm/gtx/type_trait.hpp"


class IComponentStorage
{
public:
    virtual ~IComponentStorage() = default;
};


template<typename T>
class ComponentStorage final : public IComponentStorage
{
    std::unordered_map<std::uint32_t, std::unique_ptr<T>> data;

public:
    void insert(const Entity& entity, const T& component)
    {
        data[entity.get_index()] = std::make_unique<T>(component);
    }
    void remove(const Entity& entity)
    {
        data.erase(entity.get_index());
    }
    bool contains(const Entity& entity) const
    {
        return data.contains(entity.get_index());
    }
    T& get(const Entity& entity)
    {
        assert(data.contains(entity.get_index()));

        return *data[entity.get_index()];
    }
};





#endif //COMPONENT_STORAGE_HPP
