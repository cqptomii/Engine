#ifndef EDITOR_UI_CONTEXT_HPP
#define EDITOR_UI_CONTEXT_HPP

#include <entt/entt.hpp>
#include <functional>
#include <vector>

#include "engine/scene/Scene.hpp"
#include "engine/editor/picking/manipulation_mode.hpp"

enum class PrimitiveType {
    Cube,
    Plane,
    Sphere
};

struct EditorUIContext {
    Scene& scene;
    const std::vector<entt::entity>& selected_entities;
    std::function<ManipulationMode()> get_manipulation_mode;
    std::function<void(ManipulationMode)> on_set_manipulation_mode;
    std::function<void(entt::entity)> on_select_entity;
    std::function<void()> on_clear_selection;
    std::function<entt::entity(PrimitiveType)> on_spawn_primitive;
    std::function<entt::entity(entt::entity parent)> on_create_empty_node;
    std::function<void(entt::entity)> on_delete_entity;
    std::function<void(entt::entity, entt::entity)> on_reparent_entity;
    std::function<bool(entt::entity, const std::string&)> on_rename_entity;
    std::function<void()> on_new_scene;
    std::function<bool(const std::string& path)> on_save_scene;
    std::function<bool(const std::string& path)> on_load_scene;
};

#endif
