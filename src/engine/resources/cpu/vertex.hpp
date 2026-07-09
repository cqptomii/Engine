#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 vertice; // Vertex position
    glm::vec4 color; // Vertex color
    glm::vec3 normal; // Normal associated with the vertex
    glm::vec2 texcoord; // Texture coordinate associated with the vertex
};

#endif