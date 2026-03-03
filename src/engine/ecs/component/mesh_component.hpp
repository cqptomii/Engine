//
// Created by tomfr on 03/03/2026.
//

#ifndef MESH_COMPONENT_HPP
#define MESH_COMPONENT_HPP

#include <cstdint>


class MeshComponent
{
    std::uint32_t mesh_id;

public:
    MeshComponent() = default;
    explicit MeshComponent(const std::uint32_t mesh_id) : mesh_id(mesh_id) {}
    ~MeshComponent() = default;

    std::uint32_t get_mesh_id() const noexcept
    {
        return this->mesh_id;
    }

    bool operator==(const MeshComponent& other) const
    {
        return this->mesh_id == other.mesh_id;
    }
    bool operator!=(const MeshComponent& other) const
    {
        return this->mesh_id != other.mesh_id;
    }
    explicit operator bool() const noexcept
    {
        return this->mesh_id != 0;
    }
};

#endif //MESH_COMPONENT_HPP
