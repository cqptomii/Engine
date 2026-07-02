#ifndef MESH_PRIMITIVE_3D_HPP
#define MESH_PRIMITIVE_3D_HPP

#include "engine/resources/cpu/mesh_resource.hpp"
#include "engine/resources/cpu/cpu_resource_manager.hpp"

#include <string>
#include <vector>

class MeshPrimitive3D
{
public:
    MeshPrimitive3D() = default;
    ~MeshPrimitive3D() = default;

    /**
     * @brief Create a cube mesh
     * 
     * @param resource_manager The resource manager to load the mesh
     * @param key The key of the mesh resource
     * @return ResourceHandle<MeshResource> : The handle to the mesh resource
     */
    static ResourceHandle<MeshResource> CreateCube(CpuResourceManager& resource_manager, const std::string& key = "primitive/cube/default")
    {
        std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f,  0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 0.0f,-1.0f}, {1.0f, 0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 0.0f,-1.0f}, {0.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 0.0f,-1.0f}, {0.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 0.0f,-1.0f}, {1.0f, 1.0f}}
        };
       
        std::vector<unsigned int> indices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            4, 7, 3, 3, 0, 4,
            1, 5, 6, 6, 2, 1,
            3, 2, 6, 6, 7, 3,
            4, 0, 1, 1, 5, 4
        };
        return resource_manager.load_mesh(key, vertices, indices);
    }

    /**
     * @brief Create a plane mesh
     * 
     * @param resource_manager The resource manager to load the mesh
     * @param key The key of the mesh resource
     * @return ResourceHandle<MeshResource> : The handle to the mesh resource
     */
    static ResourceHandle<MeshResource> CreatePlane(CpuResourceManager& resource_manager, const std::string& key = "primitive/plane/default")
    {
        std::vector<Vertex> vertices = {
            {{-0.5f, 0.0f, -0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, 0.0f, -0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f, 0.0f,  0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.0f,  0.5f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
        };
        std::vector<unsigned int> indices = {
            0, 1, 2, 2, 3, 0
        };

        return resource_manager.load_mesh(key, vertices, indices);
    }

    /**
     * @brief Create a UV Sphere mesh
     * 
     * @param resource_manager The resource manager to load the mesh
     * @param key The key of the mesh resource
     * @param segments The number of segments
     * @param rings The number of rings
     * @param radius The radius of the sphere
     * @return ResourceHandle<MeshResource> : The handle to the mesh resource
     */
    static ResourceHandle<MeshResource> CreateUVSphere(
        CpuResourceManager& resource_manager,
        const std::string& key = "primitive/sphere/uv",
        int segments = 32,
        int rings = 16,
        float radius = 0.5f)
    {
        segments = std::max(segments, 3);
        rings = std::max(rings, 2);
    
        std::vector<Vertex> vertices;
        vertices.reserve(static_cast<size_t>((rings + 1) * (segments + 1)));
    
        const float pi = 3.14159265358979323846f;
        const glm::vec4 color{0.8f, 0.8f, 0.8f, 1.0f};
    
        for (int ring = 0; ring <= rings; ++ring)
        {
            const float v = static_cast<float>(ring) / static_cast<float>(rings);
            const float phi = v * pi; // 0 = pôle +Y, pi = pôle -Y
    
            const float sin_phi = std::sin(phi);
            const float cos_phi = std::cos(phi);
    
            for (int seg = 0; seg <= segments; ++seg)
            {
                const float u = static_cast<float>(seg) / static_cast<float>(segments);
                const float theta = u * 2.0f * pi;
    
                const float sin_theta = std::sin(theta);
                const float cos_theta = std::cos(theta);
    
                const glm::vec3 pos{
                    radius * sin_phi * cos_theta,
                    radius * cos_phi,
                    radius * sin_phi * sin_theta
                };
    
                const glm::vec3 normal = glm::normalize(pos);
    
                vertices.push_back(Vertex{
                    pos,
                    color,
                    normal,
                    glm::vec2(u, v)
                });
            }
        }
    
        std::vector<unsigned int> indices;
        indices.reserve(static_cast<size_t>(rings * segments * 6));
    
        const auto vid = [segments](int ring, int seg) {
            return static_cast<unsigned int>(ring * (segments + 1) + seg);
        };
    
        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                const unsigned int i0 = vid(ring, seg);
                const unsigned int i1 = vid(ring + 1, seg);
                const unsigned int i2 = vid(ring + 1, seg + 1);
                const unsigned int i3 = vid(ring, seg + 1);
    
                // CCW vu de l'extérieur
                indices.push_back(i0);
                indices.push_back(i1);
                indices.push_back(i2);
    
                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }
    
        return resource_manager.load_mesh(key, vertices, indices);
    }

    /**
     * @brief Create a Ico Sphere mesh
     * 
     * @param resource_manager The resource manager to load the mesh
     * @param key The key of the mesh resource
     * @param subdivisions The number of subdivisions
     * @param radius The radius of the sphere
     * @return ResourceHandle<MeshResource> : The handle to the mesh resource
     */
    static ResourceHandle<MeshResource> CreateIcoSphere(
        CpuResourceManager& resource_manager,
        const std::string& key = "primitive/sphere/ico",
        int subdivisions = 2,
        float radius = 0.5f)
    {
        subdivisions = std::max(subdivisions, 0);
    
        const float t = (1.0f + std::sqrt(5.0f)) * 0.5f;
        const float a = 1.0f / std::sqrt(1.0f + t * t);
        const float b = t * a;
    
        std::vector<glm::vec3> positions = {
            {-a,  b, 0.0f}, { a,  b, 0.0f}, {-a, -b, 0.0f}, { a, -b, 0.0f},
            {0.0f, -a,  b   }, {0.0f,  a,  b   }, {0.0f, -a, -b   }, {0.0f,  a, -b   },
            { b, 0.0f, -a   }, { b, 0.0f,  a   }, {-b, 0.0f, -a   }, {-b, 0.0f,  a   }
        };
    
        std::vector<unsigned int> indices = {
            0, 11, 5,   0, 5, 1,   0, 1, 7,   0, 7, 10,  0, 10, 11,
            1, 5, 9,    5, 11, 4,  11, 10, 2,  10, 7, 6,   7, 1, 8,
            3, 9, 4,    3, 4, 2,    3, 2, 6,    3, 6, 8,    3, 8, 9,
            4, 9, 5,    2, 4, 11,   6, 2, 10,   8, 6, 7,    9, 8, 1
        };
    
        auto get_midpoint = [&](unsigned int i0, unsigned int i1, auto& cache) -> unsigned int {
            const unsigned int a_idx = std::min(i0, i1);
            const unsigned int b_idx = std::max(i0, i1);
            const unsigned long long key =
                (static_cast<unsigned long long>(a_idx) << 32) |
                static_cast<unsigned long long>(b_idx);
    
            if (const auto it = cache.find(key); it != cache.end())
            {
                return it->second;
            }
    
            const glm::vec3 mid = glm::normalize(positions[i0] + positions[i1]);
            positions.push_back(mid);
            const unsigned int new_index = static_cast<unsigned int>(positions.size() - 1);
            cache[key] = new_index;
            return new_index;
        };
    
        for (int level = 0; level < subdivisions; ++level)
        {
            std::unordered_map<unsigned long long, unsigned int> midpoint_cache;
            midpoint_cache.reserve(indices.size());
    
            std::vector<unsigned int> next_indices;
            next_indices.reserve(indices.size() * 4);
    
            for (size_t i = 0; i < indices.size(); i += 3)
            {
                const unsigned int i0 = indices[i + 0];
                const unsigned int i1 = indices[i + 1];
                const unsigned int i2 = indices[i + 2];
    
                const unsigned int a = get_midpoint(i0, i1, midpoint_cache);
                const unsigned int b = get_midpoint(i1, i2, midpoint_cache);
                const unsigned int c = get_midpoint(i2, i0, midpoint_cache);
    
                next_indices.insert(next_indices.end(), {
                    i0, a, c,
                    i1, b, a,
                    i2, c, b,
                    a, b, c
                });
            }
    
            indices = std::move(next_indices);
        }
    
        const glm::vec4 color{0.8f, 0.8f, 0.8f, 1.0f};
        const float pi = 3.14159265358979323846f;
    
        std::vector<Vertex> vertices;
        vertices.reserve(positions.size());
    
        for (const glm::vec3& unit_pos : positions)
        {
            const glm::vec3 pos = unit_pos * radius;
            const glm::vec3 normal = glm::normalize(unit_pos);
    
            // UV sphériques (style Blender / unwrap sphérique)
            const float u = 0.5f + std::atan2(normal.z, normal.x) / (2.0f * pi);
            const float v = 0.5f - std::asin(glm::clamp(normal.y, -1.0f, 1.0f)) / pi;
    
            vertices.push_back(Vertex{
                pos,
                color,
                normal,
                glm::vec2(u, v)
            });
        }
    
        return resource_manager.load_mesh(key, vertices, indices);
    }



};




#endif // MESH_PRIMITIVE_3D_HPP