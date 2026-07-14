#ifndef ENGINE_SCENE_SERIALIZER_HPP
#define ENGINE_SCENE_SERIALIZER_HPP

#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

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

inline constexpr int k_scene_format_version = 1;

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

inline std::vector<std::string> split_tokens(const std::string& line) {
    std::istringstream stream(line);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

inline ResourceHandle<MaterialInstance> load_default_material_instance(CpuResourceManager& resource_manager) {
    const auto default_material = resource_manager.load_material_resource(
        "material/default",
        "assets/shaders/base.vs",
        "assets/shaders/base.fs",
        {}
    );
    return resource_manager.create_material_instance(default_material);
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

        std::string mesh_path = "-";
        if (registry.all_of<MeshComponent>(entity)) {
            const ResourceHandle<MeshResource> mesh_handle = registry.get<MeshComponent>(entity).get_mesh();
            if (mesh_handle) {
                const std::string resolved_path = resource_manager.get_path_for_id(mesh_handle.get_id());
                if (!resolved_path.empty()) {
                    mesh_path = resolved_path;
                }
            }
        }

        std::string material_path = "-";
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

        output << "node"
            << ' ' << uid
            << ' ' << parent_uid
            << ' ' << (is_root ? 1 : 0)
            << ' ' << name
            << ' ' << position.x << ' ' << position.y << ' ' << position.z
            << ' ' << rotation.w << ' ' << rotation.x << ' ' << rotation.y << ' ' << rotation.z
            << ' ' << scale.x << ' ' << scale.y << ' ' << scale.z
            << ' ' << mesh_path
            << ' ' << material_path
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
    while (std::getline(input, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::vector<std::string> tokens = split_tokens(line);
        if (tokens.size() < 16 || tokens[0] != "node") {
            continue;
        }

        SceneNodeRecord record{};
        record.uid = std::stoi(tokens[1]);
        record.parent_uid = std::stoi(tokens[2]);
        record.is_root = tokens[3] == "1";
        record.name = tokens[4];
        record.position = glm::vec3(
            std::stof(tokens[5]),
            std::stof(tokens[6]),
            std::stof(tokens[7])
        );
        record.rotation = glm::quat(
            std::stof(tokens[8]),
            std::stof(tokens[9]),
            std::stof(tokens[10]),
            std::stof(tokens[11])
        );
        record.scale = glm::vec3(
            std::stof(tokens[12]),
            std::stof(tokens[13]),
            std::stof(tokens[14])
        );
        record.mesh_path = tokens[15];
        record.material_path = tokens.size() > 16 ? tokens[16] : "-";
        records.push_back(record);
    }

    if (records.empty()) {
        return false;
    }

    scene_hierarchy::clear_scene(scene);

    std::unordered_map<int, entt::entity> entity_by_uid;
    const ResourceHandle<MaterialInstance> default_material_instance = load_default_material_instance(resource_manager);

    for (const SceneNodeRecord& record : records) {
        const entt::entity entity = scene.add_object();
        entity_by_uid[record.uid] = entity;

        const DebugNameComponent debug_name{record.is_root ? scene_hierarchy::k_scene_root_name : record.name};
        scene.add_component(entity, TransformComponent{record.position, record.rotation, record.scale});
        scene.add_component(entity, debug_name);

        if (record.is_root) {
            scene.add_component(entity, SceneRootComponent{});
        }

        if (record.mesh_path != "-") {
            const ResourceHandle<MeshResource> mesh_handle = load_mesh_from_path(resource_manager, record.mesh_path);
            scene.add_component(entity, MeshComponent{mesh_handle});
            scene.add_component(entity, MaterialComponent{default_material_instance});
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
