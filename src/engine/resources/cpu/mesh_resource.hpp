/**
 * @file mesh_resource.hpp
 * @author Tom Fraisse
 * @brief Mesh Resource class
 * @version 0.1
 * @date 2026-03-09
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef ENGINE_MESH_RESOURCE_HPP
#define ENGINE_MESH_RESOURCE_HPP

#define GLM_FORCE_PACKED
#include <glm/glm.hpp>
#include <vector>
#include "engine/resources/cpu/resource_handle.hpp"
#include "engine/resources/cpu/material/material_instance.hpp"
#include "engine/resources/cpu/vertex.hpp"
#include "engine/math/aabb.hpp"

/**
 * @brief SubMesh struct
 * @details A submesh is a part of the mesh that is rendered together
 * @param index_offset The offset of the indices in the index buffer
 * @param index_count The number of indices in the submesh
 * @param material The material of the submesh
 * 
 */
struct SubMesh
{
    uint32_t index_offset;
    uint32_t index_count;

    ResourceHandle<MaterialInstance> material;
};

/**
 * @brief Mesh Resource class
 * @details A mesh resource is a collection of vertices and indices that form a mesh
 * @param vertices The vertices of the mesh
 * @param indices The indices of the mesh
 * @param submeshes The submeshes of the mesh
 * @param local_bounds The local bounds of the mesh
 * @note The local bounds is the smallest AABB that contains all the vertices of the mesh
 */
class MeshResource
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<SubMesh> submeshes;

    // Local Bounding Box of the mesh
    AABB local_bounds;

public:
    /**
     * @brief Constructor
     * @param vertices The vertices of the mesh
     * @param indices The indices of the mesh
     */
    MeshResource(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) : vertices(vertices), indices(indices) {
        std::cout << sizeof(Vertex) << std::endl;

        // Calculate the local bounds of the mesh
        local_bounds = AABB::from_vertices(vertices);
    }

    /**
     * @brief Add a submesh to the mesh
     * @param submesh The submesh to add
     */
    void add_submesh(const SubMesh& submesh)
    {
        this->submeshes.push_back(submesh);
    }


    /**
     * @brief Add a submesh to the mesh
     * @param index_offset The offset of the indices in the index buffer
     * @param index_count The number of indices in the submesh
     * @param material The material of the submesh
     */
    void add_submesh(const uint32_t index_offset, const uint32_t index_count, const ResourceHandle<MaterialInstance>& material)
    {
        SubMesh submesh{index_offset, index_count, material};
        this->submeshes.push_back(submesh);
    }

    /**
     * @brief Get the vertices of the mesh
     * @return The vertices of the mesh
     */
    std::vector<Vertex>& get_vertices()
    {
        return this->vertices;
    }


    /**
     * @brief Get the vertices of the mesh
     * @return The vertices of the mesh
     */
    const std::vector<Vertex>& get_vertices() const
    {
        return this->vertices;
    }


    /**
     * @brief Get the indices of the mesh
     * @return The indices of the mesh
     */
    std::vector<unsigned int>& get_indices()
    {
        return this->indices;
    }


    /**
     * @brief Get the indices of the mesh
     * @return The indices of the mesh
     */
    const std::vector<unsigned int>& get_indices() const
    {
        return this->indices;
    }


    /**
     * @brief Get the submeshes of the mesh
     * @return The submeshes of the mesh
     */
    std::vector<SubMesh>& get_submeshes()
    {
        return this->submeshes;
    }

    /**
     * @brief Get the local bounds of the mesh
     * @return The local bounds of the mesh
     */
    const AABB& get_local_bounds() const
    {
        return this->local_bounds;
    }
};




#endif //ENGINE_MESH_RESOURCE_HPP