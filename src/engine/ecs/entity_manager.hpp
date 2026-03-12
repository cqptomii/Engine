//
// Created by tomfr on 10/03/2026.
//

#ifndef ENGINE_ENTITY_MANAGER_HPP
#define ENGINE_ENTITY_MANAGER_HPP

#include <vector>
#include <cstdint>

#include "entity.hpp"

class EntityManager
{
    std::vector<uint32_t> generations;
    std::vector<uint32_t> free_list;
public:
    EntityManager() = default;
    ~EntityManager() = default;
    Entity create()
    {
        uint32_t id;

        // If the list is empty, add a new entity id
        if (free_list.empty())
        {
            id = generations.size();
            generations.push_back(0);
        }
        else
        {
            id = free_list.back();
            free_list.pop_back();
        }

        return {id, generations[id]};
    }
    void remove(const Entity e)
    {
        generations[e.get_index()]++;
        free_list.push_back(e.get_index());
    }
    bool is_alive(const Entity e) const
    {
        return generations[e.get_index()] == e.get_generation();
    }
};

#endif //ENGINE_ENTITY_MANAGER_HPP