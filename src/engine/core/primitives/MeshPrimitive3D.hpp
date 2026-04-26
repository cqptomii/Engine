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

};


#endif // MESH_PRIMITIVE_3D_HPP