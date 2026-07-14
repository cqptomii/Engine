#ifndef EDITOR_UI_CONTEXT_HPP
#define EDITOR_UI_CONTEXT_HPP

#include <entt/entt.hpp>
#include <functional>
#include <vector>

#include "engine/scene/Scene.hpp"

enum class PrimitiveType {
    Cube,
    Plane,
    Sphere
};

struct EditorUIContext {
    Scene& scene;
    const std::vector<entt::entity>& selected_entities;
    std::function<void(entt::entity)> on_select_entity;
    std::function<void()> on_clear_selection;
    std::function<entt::entity(PrimitiveType)> on_spawn_primitive;
    std::function<entt::entity(entt::entity parent)> on_create_empty_node;
    std::function<void(entt::entity)> on_delete_entity;
    std::function<void(entt::entity, entt::entity)> on_reparent_entity;
};

#endif
