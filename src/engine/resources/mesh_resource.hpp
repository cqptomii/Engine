//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_MESH_RESOURCE_HPP
#define ENGINE_MESH_RESOURCE_HPP

#include <glm/glm.hpp>
#include <vector>
#include "../src/engine/resources/resource_handle.hpp"
#include "../src/engine/resources/material_instance.hpp"

struct Vertex
{
    glm::vec3 vertice; // Vertex position
    glm::vec4 color; // Vertex color
    glm::vec3 normal; // Normal associated with the vertex
    glm::vec2 texcoord; // Texture coordinate associated with the vertex
};

struct SubMesh
{
    uint32_t index_offset;
    uint32_t index_count;

    ResourceHandle<MaterialInstance> material;
};

class MeshResource
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<SubMesh> submeshes;

public:
    MeshResource(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) : vertices(vertices), indices(indices) {}


    std::vector<Vertex> get_vertices() const noexcept
    {
        return this->vertices;
    }
    std::vector<unsigned int> get_indices() const noexcept
    {
        return this->indices;
    }
    std::vector<SubMesh> get_submeshes() const noexcept
    {
        return this->submeshes;
    }
};




#endif //ENGINE_MESH_RESOURCE_HPP