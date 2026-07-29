/**
 * @file scene_hierarchy.hpp
 * @author Tom FRAISSE
 * @brief Scene hierarchy functions
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
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

/**
 * @brief Scene hierarchy namespace
 * 
 */
namespace scene_hierarchy {

/**
 * @brief The name of the scene root
 */
inline constexpr const char* k_scene_root_name = "Scene";

/**
 * @brief Method to find the scene root entity
 * 
 * @param scene The scene object to find the root entity in
 * @return entt::entity The scene root entity
 */
[[nodiscard]] inline entt::entity find_root(const Scene& scene) {
    const entt::registry& registry = scene.get_registry().raw();

    // Iterate over all entities with the SceneRootComponent
    for (const auto entity : registry.view<SceneRootComponent>()) {
        return entity;
    }

    return entt::null;
}

/**
 * @brief Method to ensure the scene root entity exists
 * 
 * @param scene The scene object to ensure the root entity in
 * @return entt::entity The scene root entity
 */
[[nodiscard]] inline entt::entity ensure_root(Scene& scene) {
    // Check if the scene root entity already exists
    if (const entt::entity existing_root = find_root(scene); existing_root != entt::null) {
        return existing_root;
    }

    // Create a new scene root entity
    const entt::entity root = scene.add_object();
    const DebugNameComponent root_name{k_scene_root_name};

    // Add the SceneRootComponent, TransformComponent and DebugNameComponent to the scene root entity
    scene.add_component(root, SceneRootComponent{});
    scene.add_component(root, TransformComponent{});
    scene.add_component(root, root_name);

    // Register the scene root name with the debug system
    debug_register_name(root_name.get_id(), root_name.get_name());

    return root;
}

/**
 * @brief Method to get the parent entity of an entity
 * 
 * @param scene The scene object to get the parent entity from
 * @param entity The entity to get the parent entity from
 * @return entt::entity The parent entity
 */
[[nodiscard]] inline entt::entity get_parent(const Scene& scene, const entt::entity entity) {
    const entt::registry& registry = scene.get_registry().raw();

    // Check if the entity is valid and has a HierarchyComponent
    if (!registry.valid(entity) || !registry.all_of<HierarchyComponent>(entity)) {
        return entt::null;
    }

    return registry.get<HierarchyComponent>(entity).get_parent();
}

/**
 * @brief Method to get the children entities of a parent entity
 * 
 * @param scene The scene object to get the children entities from
 * @param parent The parent entity to get the children entities from
 * @return std::vector<entt::entity> The children entities
 */
[[nodiscard]] inline std::vector<entt::entity> get_children(const Scene& scene, const entt::entity parent) {
    std::vector<entt::entity> children;
    const entt::registry& registry = scene.get_registry().raw();

    for (const auto entity : registry.view<HierarchyComponent>()) {
        if (registry.get<HierarchyComponent>(entity).get_parent() == parent) {
            children.push_back(entity);
        }
    }

    // Sort the children entities by name
    std::sort(children.begin(), children.end(), [&registry](const entt::entity lhs, const entt::entity rhs) {
        const bool lhs_named = registry.all_of<DebugNameComponent>(lhs);
        const bool rhs_named = registry.all_of<DebugNameComponent>(rhs);
        if (lhs_named && rhs_named) {
            return registry.get<DebugNameComponent>(lhs).get_name()
                < registry.get<DebugNameComponent>(rhs).get_name();
        }
        return entt::to_integral(lhs) < entt::to_integral(rhs);
    });

    // Return the children entities
    return children;
}

/**
 * @brief Method to check if an entity is an ancestor of another entity
 * 
 * @param scene The scene object to check if the entity is an ancestor of
 * @param ancestor The ancestor entity to check if the entity is an ancestor of
 * @param entity The entity to check if it is an ancestor of
 * @return true If the entity is an ancestor of the ancestor entity
 * @return false If the entity is not an ancestor of the ancestor entity
 */
[[nodiscard]] inline bool is_ancestor_of(const Scene& scene, const entt::entity ancestor, entt::entity entity) {
    // Iterate over the parent entities of the entity until the ancestor entity is found or the entity is null
    while (entity != entt::null) {
        if (entity == ancestor) {
            return true;
        }
        entity = get_parent(scene, entity);
    }
    return false;
}

/**
 * @brief Method to check if setting a new parent would create a cycle
 * 
 * @param scene The scene object to check if setting a new parent would create a cycle
 * @param child The child entity to check if setting a new parent would create a cycle
 * @param new_parent The new parent entity to check if setting a new parent would create a cycle
 * @return true If setting a new parent would create a cycle
 * @return false If setting a new parent would not create a cycle
 */
[[nodiscard]] inline bool would_create_cycle(const Scene& scene, const entt::entity child, const entt::entity new_parent)
{
    // Check if the child or new parent is null
    if (child == entt::null || new_parent == entt::null) {
        return false;
    }
    
    // Check if the child is the new parent
    if (child == new_parent) {
        return true;
    }

    // Check if the child is an ancestor of the new parent
    return is_ancestor_of(scene, child, new_parent);
}


/**
 * @brief Method to set the parent of an entity
 * 
 * @param scene The scene object to set the parent of
 * @param child The child entity to set the parent of
 * @param parent The new parent entity to set
 */
inline void set_parent(Scene& scene, const entt::entity child, const entt::entity parent) {
    // Check if the child or new parent is null or the child is the new parent
    if (child == entt::null || child == parent) {
        return;
    }

    entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(child) || (parent != entt::null && !registry.valid(parent))) {
        return;
    }

    // Check if the new parent would create a cycle
    if (parent != entt::null && would_create_cycle(scene, child, parent)) {
        return;
    }

    // Create a new HierarchyComponent for the child if it doesn't exist
    if (registry.all_of<HierarchyComponent>(child)) {
        registry.get<HierarchyComponent>(child).set_parent(parent);
    } else {
        scene.add_component(child, HierarchyComponent{parent});
    }
}

/**
 * @brief Method to remove the parent of an entity
 * 
 * @param scene The scene object to remove the parent of
 * @param entity The entity to remove the parent of
 */
inline void remove_parent(Scene& scene, const entt::entity entity) {
    entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || !registry.all_of<HierarchyComponent>(entity)) {
        return;
    }

    // Remove the HierarchyComponent from the entity
    registry.remove<HierarchyComponent>(entity);
}


/**
 * @brief Method to get the local matrix of an entity
 * 
 * @param scene The scene object to get the local matrix of
 * @param entity The entity to get the local matrix of
 * @return glm::mat4 The local matrix of the entity
 */
[[nodiscard]] inline glm::mat4 get_local_matrix(const Scene& scene, const entt::entity entity) {
    const entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || !registry.all_of<TransformComponent>(entity)) {
        return glm::mat4(1.0f);
    }

    return registry.get<TransformComponent>(entity).get_model_matrix();
}

/**
 * @brief Method to get the world matrix of an entity
 * 
 * @param scene The scene object to get the world matrix of
 * @param entity The entity to get the world matrix of
 * @return glm::mat4 The world matrix of the entity
 */
[[nodiscard]] inline glm::mat4 get_world_matrix(const Scene& scene, const entt::entity entity) {
    const glm::mat4 local_matrix = get_local_matrix(scene, entity);
    const entt::entity parent = get_parent(scene, entity);
    if (parent == entt::null) {
        return local_matrix;
    }
    return get_world_matrix(scene, parent) * local_matrix;
}

/**
 * @brief Method to get the world position of an entity
 * 
 * @param scene The scene object to get the world position of
 * @param entity The entity to get the world position of
 * @return glm::vec3 The world position of the entity
 */
[[nodiscard]] inline glm::vec3 get_world_position(const Scene& scene, const entt::entity entity) {
    const glm::mat4 world_matrix = get_world_matrix(scene, entity);
    return glm::vec3(world_matrix[3]);
}

/**
 * @brief Method to convert a world position to a parent local position
 * 
 * @param scene The scene object to convert the world position to a parent local position
 * @param parent The parent entity to convert the world position to a parent local position
 * @param world_position The world position to convert to a parent local position
 * @return glm::vec3 The parent local position of the world position
 */
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

/**
 * @brief Method to convert a world position to a local position
 * 
 * @param scene The scene object to convert the world position to a local position
 * @param entity The entity to convert the world position to a local position
 * @param world_position The world position to convert to a local position
 * @return glm::vec3 The local position of the world position
 */
[[nodiscard]] inline glm::vec3 world_to_local_position(
    const Scene& scene,
    const entt::entity entity,
    const glm::vec3& world_position)
{
    return world_to_parent_local(scene, get_parent(scene, entity), world_position);
}

/**
 * @brief Method to set the world position of an entity
 * 
 * @param scene The scene object to set the world position of
 * @param entity The entity to set the world position of
 * @param world_position The world position to set
 */
inline void set_world_position(Scene& scene, const entt::entity entity, const glm::vec3& world_position) {
    entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || !registry.all_of<TransformComponent>(entity)) {
        return;
    }

    TransformComponent& transform = registry.get<TransformComponent>(entity);
    transform.set_position(world_to_local_position(scene, entity, world_position));
}

/**
 * @brief Method to collect the descendants of a root entity
 * 
 * @param scene The scene object to collect the descendants of
 * @param root The root entity to collect the descendants of
 * @param out The vector to collect the descendants into
 */
inline void collect_descendants(const Scene& scene, const entt::entity root, std::vector<entt::entity>& out) {
    for (const entt::entity child : get_children(scene, root)) {
        out.push_back(child);
        collect_descendants(scene, child, out);
    }
}

/**
 * @brief Method to clear the scene
 * 
 * @param scene The scene object to clear
 */
inline void clear_scene(Scene& scene) {
    scene.get_registry().raw().clear();
}

/**
 * @brief Method to delete an entity and all its descendants
 * 
 * @param scene The scene object to delete the entity and all its descendants from
 * @param entity The entity to delete and all its descendants from
 */
inline void delete_entity_cascade(Scene& scene, const entt::entity entity) {
    entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || registry.all_of<SceneRootComponent>(entity)) {
        return;
    }

    std::vector<entt::entity> to_delete;
    collect_descendants(scene, entity, to_delete);
    to_delete.push_back(entity);

    for (auto it = to_delete.rbegin(); it != to_delete.rend(); ++it) {
        if (registry.valid(*it)) {
            scene.remove_object(*it);
        }
    }
}

/**
 * @brief Method to delete an entity
 * 
 * @param scene The scene object to delete the entity from
 * @param entity The entity to delete
 */
inline void delete_entity(Scene& scene, const entt::entity entity) {
    delete_entity_cascade(scene, entity);
}

/**
 * @brief Method to make a unique name for an entity
 * 
 * @param scene The scene object to make a unique name for
 * @param base_name The base name to make a unique name for
 * @param exclude_entity The entity to exclude from the unique name
 * @return std::string The unique name
 */
[[nodiscard]] inline std::string make_unique_name(
    Scene& scene,
    const std::string& base_name,
    const entt::entity exclude_entity = entt::null)
{
    const entt::registry& registry = scene.get_registry().raw();
    bool base_used = false;
    int highest_suffix = 1;

    for (const auto entity : registry.view<DebugNameComponent>()) {
        if (entity == exclude_entity) {
            continue;
        }

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

/**
 * @brief Method to rename an entity
 * 
 * @param scene The scene object to rename the entity in
 * @param entity The entity to rename
 * @param new_name The new name for the entity
 * @return true If the entity was renamed
 * @return false If the entity was not renamed
 */
inline bool rename_entity(Scene& scene, const entt::entity entity, std::string new_name) {
    entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity) || !registry.all_of<DebugNameComponent>(entity)) {
        return false;
    }

    if (new_name.empty()) {
        return false;
    }

    if (registry.all_of<SceneRootComponent>(entity)) {
        new_name = k_scene_root_name;
    } else {
        new_name = make_unique_name(scene, new_name, entity);
    }

    DebugNameComponent& debug_name = registry.get<DebugNameComponent>(entity);
    debug_name.set_name(new_name);
    debug_register_name(debug_name.get_id(), debug_name.get_name());
    return true;
}

/**
 * @brief Method to create an empty node
 * 
 * @param scene The scene object to create the empty node in
 * @param parent The parent entity to create the empty node under
 * @param base_name The base name for the empty node
 * @return entt::entity The empty node entity
 */
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

/**
 * @brief Method to resolve the spawn parent of a selected entity
 * 
 * @param scene The scene object to resolve the spawn parent of
 * @param selected_entities The selected entities to resolve the spawn parent of
 * @return entt::entity The spawn parent entity
 */
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
