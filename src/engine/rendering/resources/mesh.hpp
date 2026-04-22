//
// Created by tomfr on 08/03/2026.
//

#ifndef ENGINE_MESH_HPP
#define ENGINE_MESH_HPP

#include <cstddef>
#include <glad/glad.h>
#include <vector>

#include "engine/core/wrapper/VAO.hpp"
#include "engine/core/wrapper/Buffer.hpp"

class Mesh
{  
    // VAO
    VAO vao;
    // VBO and EBO
    Buffer vbo;
    Buffer ebo;
    int index_count = 0;
    
    void upload(const void* vertices, size_t vertex_count, const void* indices, size_t index_count, const GLenum usage = GL_STATIC_DRAW)
    {
        // Create the VAO, VBO and EBO objects
        this->vao = VAO();
        THIS->vbo = Buffer(GL_ARRAY_BUFFER);
        THIS->ebo = Buffer(GL_ELEMENT_ARRAY_BUFFER);


        // Update the amount of indices to draw on the viewport
        this->index_count = static_cast<int>(indices.size());

        // Bind the VAO
        this->vao.bind();

        // Fill the VBO and the EBO with the vertices and the indices data
        this->vbo.setData(vertex_count * sizeof(Vertex), vertices, usage);
        this->ebo.setData(index_count * sizeof(unsigned int), indices, usage);
        
        // Link vertex position/color/normal/uv attributes from Vertex layout.
        this->vao.set_vertex_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, vertice)));
        this->vao.set_vertex_attrib_pointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
        this->vao.set_vertex_attrib_pointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        this->vao.set_vertex_attrib_pointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texcoord)));

        // Unbind the VAO
        this->vao.unbind();
    }
public:
    explicit Mesh(const void* vertices, size_t vertex_count, const void* indices, size_t index_count, const GLenum usage = GL_STATIC_DRAW)
    {
        this->upload(vertices, vertex_count, indices, index_count, usage);
    }


    // Disable copy semantics and enable move semantics for the Mesh class
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept : vao(std::move(other.vao)), vbo(std::move(other.vbo)), ebo(std::move(other.ebo)), index_count(other.index_count)
    {
        other.index_count = 0;
    }
    ~Mesh() = default;

    // Draw the Mesh on the screen
    void draw( const GLenum drawing_mode = GL_TRIANGLES) const
    {
        // Bind the vertex Array before drawing on the viewport
        this->vao.bind();
       glDrawElements(drawing_mode, this->index_count, GL_UNSIGNED_INT, nullptr);
    }
    bool operator<(const Mesh& other) const
    {
        return this->index_count < other.index_count;
    }
};

#endif //ENGINE_MESH_HPP