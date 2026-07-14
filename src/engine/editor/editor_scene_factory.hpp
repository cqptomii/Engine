#ifndef EDITOR_SCENE_FACTORY_HPP
#define EDITOR_SCENE_FACTORY_HPP

#include <entt/entt.hpp>
#include <algorithm>
#include <string>

#include <glm/glm.hpp>

#include "engine/core/debug/debug_registration.hpp"
#include "engine/scene/scene_hierarchy.hpp"
#include "engine/ecs/components/debug_name_component.hpp"
#include "engine/ecs/components/material_component.hpp"
#include "engine/ecs/components/mesh_component.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/editor/ui/editor_context.hpp"
#include "engine/resources/cpu/cpu_resource_manager.hpp"
#include "engine/resources/cpu/primitives/MeshPrimitive3D.hpp"
#include "engine/scene/Scene.hpp"

class EditorSceneFactory {
    static ResourceHandle<MaterialInstance> get_default_material_instance(CpuResourceManager& resource_manager) {
        const auto default_material = resource_manager.load_material_resource(
            "material/default",
            "assets/shaders/base.vs",
            "assets/shaders/base.fs",
            {}
        );
        return resource_manager.create_material_instance(default_material);
    }

    static ResourceHandle<MeshResource> load_mesh_for_primitive(
        CpuResourceManager& resource_manager,
        const PrimitiveType primitive_type,
        const std::string& mesh_key)
    {
        switch (primitive_type) {
            case PrimitiveType::Cube:
                return MeshPrimitive3D::CreateCube(resource_manager, mesh_key);
            case PrimitiveType::Plane:
                return MeshPrimitive3D::CreatePlane(resource_manager, mesh_key);
            case PrimitiveType::Sphere:
                return MeshPrimitive3D::CreateUVSphere(resource_manager, mesh_key);
            default:
                return MeshPrimitive3D::CreateCube(resource_manager, mesh_key);
        }
    }

    static std::string primitive_base_name(const PrimitiveType primitive_type) {
        switch (primitive_type) {
            case PrimitiveType::Cube:
                return "Cube";
            case PrimitiveType::Plane:
                return "Plane";
            case PrimitiveType::Sphere:
                return "Sphere";
            default:
                return "Object";
        }
    }

    static std::string primitive_mesh_key(const PrimitiveType primitive_type) {
        switch (primitive_type) {
            case PrimitiveType::Cube:
                return "primitive/cube/default";
            case PrimitiveType::Plane:
                return "primitive/plane/default";
            case PrimitiveType::Sphere:
                return "primitive/sphere/default";
            default:
                return "primitive/cube/default";
        }
    }

public:
    static entt::entity spawn_primitive(
        Scene& scene,
        CpuResourceManager& resource_manager,
        const PrimitiveType primitive_type,
        const glm::vec3& local_position,
        const entt::entity parent)
    {
        const std::string object_name = scene_hierarchy::make_unique_name(scene, primitive_base_name(primitive_type));
        const auto mesh_handle = load_mesh_for_primitive(
            resource_manager,
            primitive_type,
            primitive_mesh_key(primitive_type)
        );
        const auto material_instance = get_default_material_instance(resource_manager);

        const entt::entity entity = scene.add_object();
        const DebugNameComponent debug_name{object_name};

        scene.add_component(entity, TransformComponent{
            local_position,
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f)
        });
        scene.add_component(entity, debug_name);
        scene.add_component(entity, MeshComponent{mesh_handle});
        scene.add_component(entity, MaterialComponent{material_instance});
        scene_hierarchy::set_parent(scene, entity, parent);
        debug_register_name(debug_name.get_id(), debug_name.get_name());

        return entity;
    }
};

#endif
