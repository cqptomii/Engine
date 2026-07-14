#ifndef ENGINE_SCENE_HIERARCHY_HPP
#define ENGINE_SCENE_HIERARCHY_HPP

#include <algorithm>
#include <string>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "engine/core/debug/debug_registration.hpp"
#include "engine/ecs/components/debug_name_component.hpp"
#include "engine/ecs/components/hierarchy_component.hpp"
#include "engine/ecs/components/scene_root_component.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/scene/Scene.hpp"

namespace scene_hierarchy {

inline constexpr const char* k_scene_root_name = "Scene";

[[nodiscard]] inline entt::entity find_root(const Scene& scene) {
    const entt::registry& registry = scene.get_registry().raw();
    for (const auto entity : registry.view<SceneRootComponent>()) {
        return entity;
    }
    return entt::null;
}

[[nodiscard]] inline entt::entity ensure_root(Scene& scene) {
    if (const entt::entity existing_root = find_root(scene); existing_root != entt::null) {
        return existing_root;
    }

    const entt::entity root = scene.add_object();
    const DebugNameComponent root_name{k_scene_root_name};

    scene.add_component(root, SceneRootComponent{});
    scene.add_component(root, TransformComponent{});
    scene.add_component(root, root_name);
    debug_register_name(root_name.get_id(), root_name.get_name());

    return root;
}

[[nodiscard]] inline entt::entity get_parent(const Scene& scene, const entt::entity entity) {
    const entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || !registry.all_of<HierarchyComponent>(entity)) {
        return entt::null;
    }
    return registry.get<HierarchyComponent>(entity).get_parent();
}

[[nodiscard]] inline std::vector<entt::entity> get_children(const Scene& scene, const entt::entity parent) {
    std::vector<entt::entity> children;
    const entt::registry& registry = scene.get_registry().raw();

    for (const auto entity : registry.view<HierarchyComponent>()) {
        if (registry.get<HierarchyComponent>(entity).get_parent() == parent) {
            children.push_back(entity);
        }
    }

    std::sort(children.begin(), children.end(), [&registry](const entt::entity lhs, const entt::entity rhs) {
        const bool lhs_named = registry.all_of<DebugNameComponent>(lhs);
        const bool rhs_named = registry.all_of<DebugNameComponent>(rhs);
        if (lhs_named && rhs_named) {
            return registry.get<DebugNameComponent>(lhs).get_name()
                < registry.get<DebugNameComponent>(rhs).get_name();
        }
        return entt::to_integral(lhs) < entt::to_integral(rhs);
    });

    return children;
}

[[nodiscard]] inline bool is_ancestor_of(const Scene& scene, const entt::entity ancestor, entt::entity entity) {
    while (entity != entt::null) {
        if (entity == ancestor) {
            return true;
        }
        entity = get_parent(scene, entity);
    }
    return false;
}

[[nodiscard]] inline bool would_create_cycle(
    const Scene& scene,
    const entt::entity child,
    const entt::entity new_parent)
{
    if (child == entt::null || new_parent == entt::null) {
        return false;
    }
    if (child == new_parent) {
        return true;
    }
    return is_ancestor_of(scene, child, new_parent);
}

inline void set_parent(Scene& scene, const entt::entity child, const entt::entity parent) {
    if (child == entt::null || child == parent) {
        return;
    }

    entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(child) || (parent != entt::null && !registry.valid(parent))) {
        return;
    }

    if (parent != entt::null && would_create_cycle(scene, child, parent)) {
        return;
    }

    if (registry.all_of<HierarchyComponent>(child)) {
        registry.get<HierarchyComponent>(child).set_parent(parent);
    } else {
        scene.add_component(child, HierarchyComponent{parent});
    }
}

inline void remove_parent(Scene& scene, const entt::entity entity) {
    entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || !registry.all_of<HierarchyComponent>(entity)) {
        return;
    }
    registry.remove<HierarchyComponent>(entity);
}

[[nodiscard]] inline glm::mat4 get_local_matrix(const Scene& scene, const entt::entity entity) {
    const entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || !registry.all_of<TransformComponent>(entity)) {
        return glm::mat4(1.0f);
    }
    return registry.get<TransformComponent>(entity).get_model_matrix();
}

[[nodiscard]] inline glm::mat4 get_world_matrix(const Scene& scene, const entt::entity entity) {
    const glm::mat4 local_matrix = get_local_matrix(scene, entity);
    const entt::entity parent = get_parent(scene, entity);
    if (parent == entt::null) {
        return local_matrix;
    }
    return get_world_matrix(scene, parent) * local_matrix;
}

[[nodiscard]] inline glm::vec3 get_world_position(const Scene& scene, const entt::entity entity) {
    const glm::mat4 world_matrix = get_world_matrix(scene, entity);
    return glm::vec3(world_matrix[3]);
}

[[nodiscard]] inline glm::vec3 world_to_parent_local(
    const Scene& scene,
    const entt::entity parent,
    const glm::vec3& world_position)
{
    if (parent == entt::null) {
        return world_position;
    }

    const glm::mat4 parent_world = get_world_matrix(scene, parent);
    return glm::vec3(glm::inverse(parent_world) * glm::vec4(world_position, 1.0f));
}

[[nodiscard]] inline glm::vec3 world_to_local_position(
    const Scene& scene,
    const entt::entity entity,
    const glm::vec3& world_position)
{
    return world_to_parent_local(scene, get_parent(scene, entity), world_position);
}

inline void set_world_position(Scene& scene, const entt::entity entity, const glm::vec3& world_position) {
    entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || !registry.all_of<TransformComponent>(entity)) {
        return;
    }

    TransformComponent& transform = registry.get<TransformComponent>(entity);
    transform.set_position(world_to_local_position(scene, entity, world_position));
}

inline void collect_descendants(const Scene& scene, const entt::entity root, std::vector<entt::entity>& out) {
    for (const entt::entity child : get_children(scene, root)) {
        out.push_back(child);
        collect_descendants(scene, child, out);
    }
}

[[nodiscard]] inline std::string make_unique_name(Scene& scene, const std::string& base_name) {
    const entt::registry& registry = scene.get_registry().raw();
    bool base_used = false;
    int highest_suffix = 1;

    if (registry.view<DebugNameComponent>().size() == 0) {
        return base_name;
    }

    for (const auto entity : registry.view<DebugNameComponent>()) {
        const std::string& existing_name = registry.get<DebugNameComponent>(entity).get_name();
        if (existing_name == base_name) {
            base_used = true;
            continue;
        }

        const std::string prefix = base_name + " ";
        if (existing_name.rfind(prefix, 0) == 0) {
            try {
                const int suffix = std::stoi(existing_name.substr(prefix.size()));
                highest_suffix = std::max(highest_suffix, suffix);
            } catch (...) {
            }
        }
    }

    if (!base_used) {
        return base_name;
    }

    return base_name + " " + std::to_string(highest_suffix + 1);
}

[[nodiscard]] inline entt::entity create_empty_node(
    Scene& scene,
    const entt::entity parent,
    const std::string& base_name = "Empty")
{
    const entt::entity entity = scene.add_object();
    const std::string object_name = make_unique_name(scene, base_name);
    const DebugNameComponent debug_name{object_name};

    scene.add_component(entity, TransformComponent{});
    scene.add_component(entity, debug_name);
    set_parent(scene, entity, parent);
    debug_register_name(debug_name.get_id(), debug_name.get_name());

    return entity;
}

inline void delete_entity(Scene& scene, const entt::entity entity) {
    entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || registry.all_of<SceneRootComponent>(entity)) {
        return;
    }

    const entt::entity parent = get_parent(scene, entity);
    for (const entt::entity child : get_children(scene, entity)) {
        set_parent(scene, child, parent);
    }

    scene.remove_object(entity);
}

[[nodiscard]] inline entt::entity resolve_spawn_parent(
    Scene& scene,
    const std::vector<entt::entity>& selected_entities)
{
    const entt::entity root = ensure_root(scene);
    if (selected_entities.empty()) {
        return root;
    }

    const entt::entity selected = selected_entities.front();
    const entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(selected)) {
        return root;
    }

    return selected;
}

} // namespace scene_hierarchy

#endif
