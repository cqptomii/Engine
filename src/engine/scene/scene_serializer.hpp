/**
 * @file scene_serializer.hpp
 * @author Tom FRAISSE
 * @brief Text serialization of the editor scene graph
 * @version 0.2
 * @date 2026-07-29
 *
 * @copyright Copyright (c) 2026
 *
 * @details File format:
 * @code
 * # GameEngine Scene v2
 * node <uid> <parent_uid> <is_root> "<name>" <pos.x y z> <rot.w x y z> <scale.x y z> "<mesh>" "<material>"
 * @endcode
 * - the header line is mandatory and carries the format version;
 * - `uid` is a save-local index, `parent_uid` is -1 for a node without parent;
 * - `name`, `mesh` and `material` are quoted so they may contain spaces, `"-"` means absent;
 * - inside a quoted field, `\` escapes the next character;
 * - lines starting with `#` are comments, malformed lines are skipped with a warning.
 *
 * Version history:
 * - v1: same fields, unquoted (names and paths containing spaces were unreadable);
 * - v2: quoted `name` / `mesh` / `material`. v1 files still load.
 *
 * Known limitation: shader and texture paths are not serialized, so a material can only be
 * restored when its resource is already in the CPU cache; otherwise the node falls back to
 * the default material.
 */

#ifndef ENGINE_SCENE_SERIALIZER_HPP
#define ENGINE_SCENE_SERIALIZER_HPP

#include <cctype>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "engine/core/debug/debug_config.hpp"
#include "engine/core/debug/debug_registration.hpp"
#include "engine/ecs/components/debug_name_component.hpp"
#include "engine/ecs/components/hierarchy_component.hpp"
#include "engine/ecs/components/material_component.hpp"
#include "engine/ecs/components/mesh_component.hpp"
#include "engine/ecs/components/scene_root_component.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/resources/cpu/cpu_resource_manager.hpp"
#include "engine/resources/cpu/primitives/MeshPrimitive3D.hpp"
#include "engine/scene/scene_hierarchy.hpp"
#include "engine/scene/Scene.hpp"

namespace scene_serializer {

/// Version written by save_scene().
inline constexpr int k_scene_format_version = 2;

/// Oldest version load_scene() still accepts.
inline constexpr int k_min_scene_format_version = 1;

/// Marker written in place of an absent mesh or material path.
inline constexpr const char* k_empty_field = "-";

/// Resource path of the fallback material.
inline constexpr const char* k_default_material_path = "material/default";

struct SceneNodeRecord {
    int uid = -1;
    int parent_uid = -1;
    std::string name;
    bool is_root = false;
    glm::vec3 position{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};
    std::string mesh_path;
    std::string material_path;
};

inline std::string trim(std::string value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
        value.erase(value.begin());
    }
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) {
        value.pop_back();
    }
    return value;
}

/**
 * @brief Wrap a field in quotes so it survives a round trip even with spaces.
 *
 * @param value The raw field value
 * @return std::string The quoted field, with `\` and `"` escaped
 */
inline std::string quote_field(const std::string& value) {
    std::string quoted;
    quoted.reserve(value.size() + 2);
    quoted.push_back('"');

    for (const char character : value) {
        if (character == '"' || character == '\\') {
            quoted.push_back('\\');
        }
        quoted.push_back(character);
    }

    quoted.push_back('"');
    return quoted;
}

/**
 * @brief Split a line into fields, keeping quoted fields in one piece.
 *
 * @param line The line to split
 * @return std::vector<std::string> The fields, unquoted and unescaped
 * @details Unquoted fields are split on whitespace, which is how v1 files are still read.
 */
inline std::vector<std::string> split_tokens(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_quotes = false;
    bool token_started = false;

    for (std::size_t index = 0; index < line.size(); ++index) {
        const char character = line[index];

        if (in_quotes) {
            // Inside quotes only the escape character is special, spaces are kept.
            if (character == '\\' && index + 1 < line.size()) {
                token.push_back(line[++index]);
                continue;
            }
            if (character == '"') {
                in_quotes = false;
                continue;
            }
            token.push_back(character);
            continue;
        }

        if (character == '"') {
            in_quotes = true;
            // An empty quoted field must still produce a token.
            token_started = true;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(character))) {
            if (token_started) {
                tokens.push_back(token);
                token.clear();
                token_started = false;
            }
            continue;
        }

        token.push_back(character);
        token_started = true;
    }

    if (token_started) {
        tokens.push_back(token);
    }

    return tokens;
}

/**
 * @brief Convert a field to an int without throwing on malformed input.
 *
 * @param token The field to convert
 * @param out_value Receives the parsed value on success
 * @return true If the whole field was a valid int
 */
inline bool parse_int(const std::string& token, int& out_value) {
    try {
        std::size_t consumed = 0;
        const int value = std::stoi(token, &consumed);
        if (consumed != token.size()) {
            return false;
        }
        out_value = value;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

/**
 * @brief Convert a field to a float without throwing on malformed input.
 *
 * @param token The field to convert
 * @param out_value Receives the parsed value on success
 * @return true If the whole field was a valid float
 */
inline bool parse_float(const std::string& token, float& out_value) {
    try {
        std::size_t consumed = 0;
        const float value = std::stof(token, &consumed);
        if (consumed != token.size()) {
            return false;
        }
        out_value = value;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

/**
 * @brief Read the format version out of the header comment.
 *
 * @param comment_line A line starting with `#`
 * @param out_version Receives the version on success
 * @return true If the line carried a `v<number>` marker
 */
inline bool parse_scene_version(const std::string& comment_line, int& out_version) {
    const std::size_t marker = comment_line.rfind(" v");
    if (marker == std::string::npos) {
        return false;
    }

    return parse_int(comment_line.substr(marker + 2), out_version);
}

/**
 * @brief Parse a `node` line into a record.
 *
 * @param tokens The fields of the line
 * @param record Receives the parsed node
 * @return true If every field was valid
 */
inline bool parse_node_record(const std::vector<std::string>& tokens, SceneNodeRecord& record) {
    if (tokens.size() < 16 || tokens[0] != "node") {
        return false;
    }

    if (!parse_int(tokens[1], record.uid) || !parse_int(tokens[2], record.parent_uid)) {
        return false;
    }

    record.is_root = tokens[3] == "1";
    record.name = tokens[4].empty() ? "Entity" : tokens[4];

    // Position (3), rotation as w/x/y/z (4) then scale (3).
    float transform_values[10] = {};
    for (std::size_t index = 0; index < 10; ++index) {
        if (!parse_float(tokens[5 + index], transform_values[index])) {
            return false;
        }
    }

    record.position = glm::vec3(transform_values[0], transform_values[1], transform_values[2]);
    record.rotation = glm::quat(transform_values[3], transform_values[4], transform_values[5], transform_values[6]);
    record.scale = glm::vec3(transform_values[7], transform_values[8], transform_values[9]);

    record.mesh_path = tokens[15];
    record.material_path = tokens.size() > 16 ? tokens[16] : k_empty_field;
    return true;
}

inline ResourceHandle<MaterialInstance> load_default_material_instance(CpuResourceManager& resource_manager) {
    const auto default_material = resource_manager.load_material_resource(
        k_default_material_path,
        "assets/shaders/base.vs",
        "assets/shaders/base.fs",
        {}
    );
    return resource_manager.create_material_instance(default_material);
}

/**
 * @brief Resolve the material instance of a node from its saved resource path.
 *
 * @param resource_manager The CPU resource manager
 * @param material_path The saved material resource path
 * @param default_instance Fallback instance used when the material cannot be restored
 * @param instance_by_path Cache so nodes sharing a material share one instance
 * @return ResourceHandle<MaterialInstance> The instance to attach to the node
 * @details Shader and texture paths are not serialized, so only a material already present in
 * the CPU cache can be restored; anything else falls back to @p default_instance.
 */
inline ResourceHandle<MaterialInstance> resolve_material_instance(
    CpuResourceManager& resource_manager,
    const std::string& material_path,
    const ResourceHandle<MaterialInstance> default_instance,
    std::unordered_map<std::string, ResourceHandle<MaterialInstance>>& instance_by_path)
{
    if (material_path.empty() || material_path == k_empty_field) {
        return default_instance;
    }

    const auto cached = instance_by_path.find(material_path);
    if (cached != instance_by_path.end()) {
        return cached->second;
    }

    if (!resource_manager.is_material_loaded(material_path)) {
        ENGINE_LOG(LogLevel::Warning, "scene",
            "Material not in cache, falling back to default: " + material_path);
        return default_instance;
    }

    const ResourceHandle<MaterialResource> material_resource(
        resource_manager.compute_resource_id(material_path)
    );
    const ResourceHandle<MaterialInstance> instance = resource_manager.create_material_instance(material_resource);
    instance_by_path.emplace(material_path, instance);
    return instance;
}

inline ResourceHandle<MeshResource> load_mesh_from_path(
    CpuResourceManager& resource_manager,
    const std::string& mesh_path)
{
    if (mesh_path == "primitive/cube/default") {
        return MeshPrimitive3D::CreateCube(resource_manager, mesh_path);
    }
    if (mesh_path == "primitive/plane/default") {
        return MeshPrimitive3D::CreatePlane(resource_manager, mesh_path);
    }
    if (mesh_path == "primitive/sphere/default") {
        return MeshPrimitive3D::CreateUVSphere(resource_manager, mesh_path);
    }

    if (resource_manager.is_mesh_loaded(mesh_path)) {
        return ResourceHandle<MeshResource>(resource_manager.compute_resource_id(mesh_path));
    }

    return MeshPrimitive3D::CreateCube(resource_manager, mesh_path);
}

inline bool save_scene(const Scene& scene, CpuResourceManager& resource_manager, const std::string& file_path) {
    const entt::registry& registry = scene.get_registry().raw();
    const entt::entity root = scene_hierarchy::find_root(scene);
    if (root == entt::null) {
        return false;
    }

    std::unordered_map<entt::entity, int> uid_by_entity;
    std::vector<entt::entity> entities;
    entities.push_back(root);

    std::vector<entt::entity> descendants;
    scene_hierarchy::collect_descendants(scene, root, descendants);
    entities.insert(entities.end(), descendants.begin(), descendants.end());

    int next_uid = 0;
    for (const entt::entity entity : entities) {
        uid_by_entity[entity] = next_uid++;
    }

    std::filesystem::create_directories(std::filesystem::path(file_path).parent_path());

    std::ofstream output(file_path, std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    output << "# GameEngine Scene v" << k_scene_format_version << '\n';

    for (const entt::entity entity : entities) {
        if (!registry.valid(entity)) {
            continue;
        }

        const int uid = uid_by_entity.at(entity);
        const entt::entity parent = scene_hierarchy::get_parent(scene, entity);
        const int parent_uid = parent != entt::null && uid_by_entity.contains(parent)
            ? uid_by_entity.at(parent)
            : -1;

        const bool is_root = registry.all_of<SceneRootComponent>(entity);
        const std::string name = registry.all_of<DebugNameComponent>(entity)
            ? registry.get<DebugNameComponent>(entity).get_name()
            : "Entity";

        glm::vec3 position{0.0f};
        glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 scale{1.0f};
        if (registry.all_of<TransformComponent>(entity)) {
            const TransformComponent& transform = registry.get<TransformComponent>(entity);
            position = transform.get_position();
            rotation = transform.get_rotation();
            scale = transform.get_scale();
        }

        std::string mesh_path = k_empty_field;
        if (registry.all_of<MeshComponent>(entity)) {
            const ResourceHandle<MeshResource> mesh_handle = registry.get<MeshComponent>(entity).get_mesh();
            if (mesh_handle) {
                const std::string resolved_path = resource_manager.get_path_for_id(mesh_handle.get_id());
                if (!resolved_path.empty()) {
                    mesh_path = resolved_path;
                }
            }
        }

        std::string material_path = k_empty_field;
        if (registry.all_of<MaterialComponent>(entity)) {
            const ResourceHandle<MaterialInstance> material_handle = registry.get<MaterialComponent>(entity).get_material();
            if (material_handle) {
                const MaterialInstance& material_instance = resource_manager.get_material_instance(material_handle);
                const ResourceHandle<MaterialResource> material_resource = material_instance.get_material_resource();
                if (material_resource) {
                    const std::string resolved_path = resource_manager.get_path_for_id(material_resource.get_id());
                    if (!resolved_path.empty()) {
                        material_path = resolved_path;
                    }
                }
            }
        }

        // Name and paths are quoted: they may contain spaces, unlike the numeric fields.
        output << "node"
            << ' ' << uid
            << ' ' << parent_uid
            << ' ' << (is_root ? 1 : 0)
            << ' ' << quote_field(name)
            << ' ' << position.x << ' ' << position.y << ' ' << position.z
            << ' ' << rotation.w << ' ' << rotation.x << ' ' << rotation.y << ' ' << rotation.z
            << ' ' << scale.x << ' ' << scale.y << ' ' << scale.z
            << ' ' << quote_field(mesh_path)
            << ' ' << quote_field(material_path)
            << '\n';
    }

    return true;
}

inline bool load_scene(Scene& scene, CpuResourceManager& resource_manager, const std::string& file_path) {
    std::ifstream input(file_path);
    if (!input.is_open()) {
        return false;
    }

    std::vector<SceneNodeRecord> records;
    std::string line;
    int line_number = 0;
    bool version_found = false;

    while (std::getline(input, line)) {
        ++line_number;
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        if (line[0] == '#') {
            int file_version = 0;
            if (!version_found && parse_scene_version(line, file_version)) {
                version_found = true;

                // Refuse a file written by a newer engine instead of misreading its fields.
                if (file_version < k_min_scene_format_version || file_version > k_scene_format_version) {
                    ENGINE_LOG(LogLevel::Error, "scene",
                        "Unsupported scene format version " + std::to_string(file_version)
                        + " in " + file_path);
                    return false;
                }
            }
            continue;
        }

        if (!version_found) {
            ENGINE_LOG(LogLevel::Error, "scene", "Missing scene format header in " + file_path);
            return false;
        }

        SceneNodeRecord record{};
        if (!parse_node_record(split_tokens(line), record)) {
            // A corrupted line costs one node, not the whole load.
            ENGINE_LOG(LogLevel::Warning, "scene",
                "Skipped malformed scene line " + std::to_string(line_number) + " in " + file_path);
            continue;
        }

        records.push_back(record);
    }

    if (records.empty()) {
        ENGINE_LOG(LogLevel::Error, "scene", "No readable node in " + file_path);
        return false;
    }

    scene_hierarchy::clear_scene(scene);

    std::unordered_map<int, entt::entity> entity_by_uid;
    const ResourceHandle<MaterialInstance> default_material_instance = load_default_material_instance(resource_manager);

    // Nodes sharing a material path share a single instance; the default material is
    // pre-seeded since load_default_material_instance() just created it.
    std::unordered_map<std::string, ResourceHandle<MaterialInstance>> instance_by_material_path;
    instance_by_material_path.emplace(k_default_material_path, default_material_instance);

    for (const SceneNodeRecord& record : records) {
        const entt::entity entity = scene.add_object();
        entity_by_uid[record.uid] = entity;

        const DebugNameComponent debug_name{record.is_root ? scene_hierarchy::k_scene_root_name : record.name};
        scene.add_component(entity, TransformComponent{record.position, record.rotation, record.scale});
        scene.add_component(entity, debug_name);

        if (record.is_root) {
            scene.add_component(entity, SceneRootComponent{});
        }

        if (record.mesh_path != k_empty_field) {
            const ResourceHandle<MeshResource> mesh_handle = load_mesh_from_path(resource_manager, record.mesh_path);
            const ResourceHandle<MaterialInstance> material_instance = resolve_material_instance(
                resource_manager,
                record.material_path,
                default_material_instance,
                instance_by_material_path
            );

            scene.add_component(entity, MeshComponent{mesh_handle});
            scene.add_component(entity, MaterialComponent{material_instance});
        }

        debug_register_name(debug_name.get_id(), debug_name.get_name());
    }

    for (const SceneNodeRecord& record : records) {
        const entt::entity entity = entity_by_uid.at(record.uid);
        if (record.parent_uid >= 0 && entity_by_uid.contains(record.parent_uid)) {
            scene_hierarchy::set_parent(scene, entity, entity_by_uid.at(record.parent_uid));
        }
    }

    (void)scene_hierarchy::ensure_root(scene);
    return true;
}

} // namespace scene_serializer

#endif
