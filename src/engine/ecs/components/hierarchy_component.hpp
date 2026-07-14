#ifndef ENGINE_HIERARCHY_COMPONENT_HPP
#define ENGINE_HIERARCHY_COMPONENT_HPP

#include <entt/entt.hpp>

class HierarchyComponent {
    entt::entity parent = entt::null;

public:
    HierarchyComponent() = default;

    explicit HierarchyComponent(const entt::entity parent_entity) : parent(parent_entity) {}

    [[nodiscard]] entt::entity get_parent() const noexcept {
        return this->parent;
    }

    void set_parent(const entt::entity parent_entity) noexcept {
        this->parent = parent_entity;
    }

    [[nodiscard]] bool has_parent() const noexcept {
        return this->parent != entt::null;
    }
};

#endif
