//
// Created by tomfr on 25/04/2026.
//

#ifndef ENGINE_MESH_COMPONENT_HPP
#define ENGINE_MESH_COMPONENT_HPP

#include "engine/resources/cpu/mesh_resource.hpp"
#include "engine/resources/cpu/resource_handle.hpp"

class MeshComponent
{
    ResourceHandle<MeshResource> mesh;

public:
    explicit MeshComponent(const ResourceHandle<MeshResource> mesh) : mesh(mesh) {}

    ResourceHandle<MeshResource> get_mesh() const
    {
        return this->mesh;
    }
};

#endif //ENGINE_MESH_COMPONENT_HPP
