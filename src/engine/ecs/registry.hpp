//
// Created by tomfr on 03/03/2026.
//

#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <utility>
#include <entt/entt.hpp>

class Registry
{
    entt::registry entt_registry;

public:
    Registry() = default;

    entt::entity create_entity()
    {
        return this->entt_registry.create();
    }

    void remove_entity(const entt::entity entity)
    {
        if (!this->entt_registry.valid(entity))
        {
            return;
        }

        this->entt_registry.destroy(entity);
    }

    template<typename T>
    decltype(auto) storage()
    {
        return this->entt_registry.storage<T>();
    }

    template<typename T>
    void add(const entt::entity entity, T component)
    {
        this->entt_registry.emplace_or_replace<T>(entity, std::move(component));
    }

    template<typename T>
    void remove(const entt::entity entity)
    {
        if (!this->entt_registry.valid(entity))
        {
            return;
        }

        this->entt_registry.remove<T>(entity);
    }

    template<typename T>
    T& get(const entt::entity entity)
    {
        return this->entt_registry.get<T>(entity);
    }

    template<typename First, typename... Rest, typename Func>
    void view(Func&& func)
    {
        auto ecs_view = this->entt_registry.view<First, Rest...>();

        for (const entt::entity entt_entity : ecs_view)
        {
            func(entt_entity, ecs_view.template get<First>(entt_entity), ecs_view.template get<Rest>(entt_entity)...);
        }
    }

    entt::registry& raw()
    {
        return this->entt_registry;
    }

    [[nodiscard]] const entt::registry& raw() const
    {
        return this->entt_registry;
    }
};

#endif //REGISTRY_HPP
