//
// Created by tomfr on 08/03/2026.
//

#ifndef ENGINE_MESH_HPP
#define ENGINE_MESH_HPP

#include <glad/glad.h>
#include <vector>
#include "../src/engine/resources/mesh_resource.hpp"


class Mesh
{
    unsigned int vao{}; // Vertex Array
    unsigned int vbo{}; // Vertex Buffer
    unsigned int ebo{}; // Element Buffer
    int triangle_amount = 0;

    template<typename T>
    static unsigned int make_buffer( std::vector<T> vertices, const unsigned int buffer_type, const GLenum usage)
    {
        unsigned int buffer;
        // Generate the Vertex Buffer and bind it
        glGenBuffers(1, &buffer);
        glBindBuffer(buffer_type, buffer);

        // link the vertices to the Vertex Buffer
        glBufferData(buffer_type, vertices.size() * sizeof(T), vertices.data(), usage);

        // unbind the Vertex Buffer
        glBindBuffer(buffer_type, 0);

        return buffer;
    }

    void upload(const MeshResource& mesh_resource, const GLenum usage = GL_STATIC_DRAW)
    {

        // Generate the vertex array
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        const auto vertices = mesh_resource.get_vertices();
        const auto indices = mesh_resource.get_indices();

        // Update the amount of triangle to draw on the viewport
        this->triangle_amount = static_cast<int>(vertices.size()) / 3;

        // Make the vertex buffer and the element Buffer
        vbo = make_buffer(vertices, GL_ARRAY_BUFFER, usage);
        ebo = make_buffer(indices, GL_ELEMENT_ARRAY_BUFFER, usage);

        // Link the vertices within the vertex Array
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
        glBindVertexArray(0);
    }
public:
    explicit Mesh(const MeshResource& mesh_resource, const GLenum usage = GL_STATIC_DRAW)
    {
        this->upload(mesh_resource, usage);
    }
    ~Mesh()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    };

    // Draw the Mesh on the screen
    void draw( const GLenum drawing_mode = GL_TRIANGLES) const
    {
        // Bind the vertex Array before drawing on the viewport
        glBindVertexArray(this->vao);
        glDrawElements(drawing_mode, this->triangle_amount, GL_UNSIGNED_INT, nullptr);
    }
};

#endif //ENGINE_MESH_HPP