#ifndef MESH_PRIMITIVE_2D_HPP
#define MESH_PRIMITIVE_2D_HPP

#include "engine/resources/cpu/mesh_resource.hpp"

class MeshPrimitive2D
{
public:
    MeshPrimitive2D() = default;
    ~MeshPrimitive2D() = default;

    static MeshResource* CreateQuad(){
        std::vector<float> vertices = {
            // Positions       // UVs
            -0.5f, -0.5f,     0.0f, 0.0f,
             0.5f, -0.5f,     1.0f, 0.0f,
             0.5f,  0.5f,     1.0f, 1.0f,
            -0.5f,  0.5f,     0.0f, 1.0f
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };

        return new MeshResource(vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size() * sizeof(unsigned int));
    }
    static MeshResource* CreateCircle(unsigned int segments = 32){
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // Center vertex
        vertices.push_back(0.0f); // Position
        vertices.push_back(0.0f);
        vertices.push_back(0.5f); // UV
        vertices.push_back(0.5f);

        for (unsigned int i = 0; i <= segments; ++i) {
            float angle = (float)i / (float)segments * 2.0f * M_PI;
            float xPos = std::cos(angle) * 0.5f;
            float yPos = std::sin(angle) * 0.5f;

            vertices.push_back(xPos); // Position
            vertices.push_back(yPos);
            vertices.push_back((std::cos(angle) + 1.0f) * 0.5f); // UV
            vertices.push_back((std::sin(angle) + 1.0f) * 0.5f);

            if (i > 0) {
                indices.push_back(0); // Center vertex
                indices.push_back(i);
                indices.push_back(i + 1);
            }
        }

         return new MeshResource(vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size() * sizeof(unsigned int));
    }
    static MeshResource* CreatePolygon(unsigned int sides){
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // Center vertex
        vertices.push_back(0.0f); // Position
        vertices.push_back(0.0f);
        vertices.push_back(0.5f); // UV
        vertices.push_back(0.5f);

        for (unsigned int i = 0; i < sides; ++i) {
            float angle = (float)i / (float)sides * 2.0f * M_PI;
            float xPos = std::cos(angle) * 0.5f;
            float yPos = std::sin(angle) * 0.5f;

            vertices.push_back(xPos); // Position
            vertices.push_back(yPos);
            vertices.push_back((std::cos(angle) + 1.0f) * 0.5f); // UV
            vertices.push_back((std::sin(angle) + 1.0f) * 0.5f);

            if (i > 0) {
                indices.push_back(0); // Center vertex
                indices.push_back(i);
                indices.push_back(i + 1);
            }
        }

        // Close the polygon by connecting the last vertex to the first
         indices.push_back(0); // Center vertex
         indices.push_back(sides);
         indices.push_back(1);

         return new MeshResource(vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size() * sizeof(unsigned int));
    }
};


#endif // MESH_PRIMITIVE_2D_HPP