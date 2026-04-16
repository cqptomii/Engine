//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_MESH_RESOURCE_HPP
#define ENGINE_MESH_RESOURCE_HPP

#include <glm/glm.hpp>
#include <vector>
#include "../src/engine/resources/resource_handle.hpp"
#include "../src/engine/resources/material/material_instance.hpp"

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

    void add_submesh(const SubMesh& submesh)
    {
        this->submeshes.push_back(submesh);
    }
    void add_submesh(const uint32_t index_offset, const uint32_t index_count, const ResourceHandle<MaterialInstance>& material)
    {
        SubMesh submesh{index_offset, index_count, material};
        this->submeshes.push_back(submesh);
    }

    std::vector<Vertex>& get_vertices()
    {
        return this->vertices;
    }
    const std::vector<Vertex>& get_vertices() const
    {
        return this->vertices;
    }
    std::vector<unsigned int>& get_indices()
    {
        return this->indices;
    }
    const std::vector<unsigned int>& get_indices() const
    {
        return this->indices;
    }
    std::vector<SubMesh>& get_submeshes()
    {
        return this->submeshes;
    }
};




#endif //ENGINE_MESH_RESOURCE_HPP