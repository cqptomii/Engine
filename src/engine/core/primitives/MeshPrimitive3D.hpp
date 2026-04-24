#ifndef MESH_PRIMITIVE_3D_HPP
#define MESH_PRIMITIVE_3D_HPP

#include "engine/resources/cpu/mesh_resource.hpp"
#include "engine/core/wrapper/VAO.hpp"
#include "engine/core/wrapper/Buffer.hpp"
#include <cmath>

class MeshPrimitive3D
{
public:
    MeshPrimitive3D() = default;
    ~MeshPrimitive3D() = default;

    static MeshResource* CreateCube(){
        // Define vertices for a cube
        std::vector<float> vertices = {
            // Front face
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            // Back face
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        };

        // Define indices for the cube
        std::vector<unsigned int> indices = {
            // Front face
            0, 1, 2,
            2, 3, 0,
            // Back face
            4, 5, 6,
            6, 7, 4,
            // Left face
            4, 7, 3,
            3, 2, 4,
            // Right face
            1, 6, 7,
            7, 4, 1,
            // Top face
            3, 2, 6,
            6, 7, 3,
            // Bottom face
            4, 1, 2,
            2, 3, 4
        };

        return new MeshResource(vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size() * sizeof(unsigned int));
    }
    static MeshResource* CreateSphere(unsigned int segments = 32){
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int y = 0; y <= segments; ++y) {
            for (unsigned int x = 0; x <= segments; ++x) {
                float xSegment = (float)x / (float)segments;
                float ySegment = (float)y / (float)segments;
                float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
                float yPos = std::cos(ySegment * M_PI);
                float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);
                vertices.push_back(xSegment);
                vertices.push_back(ySegment);
            }
        }

        for (unsigned int y = 0; y < segments; ++y) {
            for (unsigned int x = 0; x < segments; ++x) {
                unsigned int first = (y * (segments + 1)) + x;
                unsigned int second = first + segments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        return new MeshResource(vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size() * sizeof(unsigned int));
    }
    static MeshResource* CreateCylinder(unsigned int segments = 32){
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
    
            for (unsigned int i = 0; i <= segments; ++i) {
                float angle = (float)i / (float)segments * 2.0f * M_PI;
                float xPos = std::cos(angle);
                float zPos = std::sin(angle);
    
                // Top circle vertex
                vertices.push_back(xPos);
                vertices.push_back(0.5f);
                vertices.push_back(zPos);
                vertices.push_back((float)i / (float)segments);
                vertices.push_back(1.0f);
    
                // Bottom circle vertex
                vertices.push_back(xPos);
                vertices.push_back(-0.5f);
                vertices.push_back(zPos);
                vertices.push_back((float)i / (float)segments);
                vertices.push_back(0.0f);
            }
    
            for (unsigned int i = 0; i < segments; ++i) {
                unsigned int top1 = i * 2;
                unsigned int bottom1 = top1 + 1;
                unsigned int top2 = ((i + 1) % (segments + 1)) * 2;
                unsigned int bottom2 = top2 + 1;
    
                // Side faces
                indices.push_back(top1);
                indices.push_back(bottom1);
                indices.push_back(top2);
    
                indices.push_back(bottom1);
                indices.push_back(bottom2);
                indices.push_back(top2);
    
                // Top face
                indices.push_back(top1);
                indices.push_back(top2);
                indices.push_back(segments * 2); // Center vertex
    
                // Bottom face
                indices.push_back(bottom1);
                indices.push_back(segments * 2 + 1); // Center vertex
                indices.push_back(bottom2);
            }
    
            // Add center vertices for top and bottom faces
            vertices.push_back(0.0f); // Top center vertex
            vertices.push_back(0.5f);
            vertices.push_back(0.0f);
            vertices.push_back(0.5f);
            vertices.push_back(1.0f);
    
            vertices.push_back(0.0f); // Bottom center vertex
            vertices.push_back(-0.5f);
            vertices.push_back(0.0f);
            vertices.push_back(0.5f);
            vertices.push_back(0.0f);
        
    }
    static MeshResource* CreateCone(unsigned int segments = 32){
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i <= segments; ++i) {
            float angle = (float)i / (float)segments * 2.0f * M_PI;
            float xPos = std::cos(angle);
            float zPos = std::sin(angle);

            // Base circle vertex
            vertices.push_back(xPos);
            vertices.push_back(0.0f);
            vertices.push_back(zPos);
            vertices.push_back((float)i / (float)segments);
            vertices.push_back(0.0f);
        }

        // Apex vertex
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(1.0f);

        for (unsigned int i = 0; i < segments; ++i) {
            unsigned int base1 = i;
            unsigned int base2 = (i + 1) % (segments + 1);
            unsigned int apex = segments + 1;

            // Side face
            indices.push_back(base1);
            indices.push_back(base2);
            indices.push_back(apex);

            // Base face
            indices.push_back(base1);
            indices.push_back(base2);
            indices.push_back(segments + 2); // Center vertex
        }

        // Add center vertex for base face
        vertices.push_back(0.0f); // Base center vertex
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.0f);

         return new MeshResource(vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size() * sizeof(unsigned int));
    }
    static MeshResource* CreateTorus(unsigned int segments = 32, unsigned int rings = 16){
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        float radius = 0.5f;
        float tubeRadius = 0.2f;

        for (unsigned int i = 0; i <= segments; ++i) {
            float segmentAngle = (float)i / (float)segments * 2.0f * M_PI;
            for (unsigned int j = 0; j <= rings; ++j) {
                float ringAngle = (float)j / (float)rings * 2.0f * M_PI;

                float xPos = (radius + tubeRadius * std::cos(ringAngle)) * std::cos(segmentAngle);
                float yPos = tubeRadius * std::sin(ringAngle);
                float zPos = (radius + tubeRadius * std::cos(ringAngle)) * std::sin(segmentAngle);

                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);
                vertices.push_back((float)i / (float)segments);
                vertices.push_back((float)j / (float)rings);
            }
        }

        for (unsigned int i = 0; i < segments; ++i) {
            for (unsigned int j = 0; j < rings; ++j) {
                unsigned int first = (i * (rings + 1)) + j;
                unsigned int second = first + rings + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

         return new MeshResource(vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size() * sizeof(unsigned int));
    }

};


#endif // MESH_PRIMITIVE_3D_HPP