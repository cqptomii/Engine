//
// Created by tomfr on 08/03/2026.
//

#ifndef ENGINE_MESH_HPP
#define ENGINE_MESH_HPP

#include <cstddef>
#include <glad/glad.h>
#include <vector>

#include "engine/resources/gpu/wrappers/VAO.hpp"
#include "engine/resources/gpu/wrappers/Buffer.hpp"
#include "engine/resources/cpu/mesh_resource.hpp"
#include "engine/core/debug/debug_hooks.hpp"

class Mesh
{  
    // VAO
    VAO vao;
    // VBO and EBO
    Buffer vbo{GL_ARRAY_BUFFER};
    Buffer ebo{GL_ELEMENT_ARRAY_BUFFER};
    int index_count = 0;
    
    void upload(const void* vertices, size_t vertex_count, const void* indices, size_t index_count, const GLenum usage = GL_STATIC_DRAW)
    {
        // Update the amount of indices to draw on the viewport
        this->index_count = static_cast<int>(index_count);
        printf("Mesh: Uploading %zu vertices and %zu indices to the GPU.\n", vertex_count, index_count);

        // Bind the VAO
        this->vao.bind();
        this->vbo.bind();
        this->ebo.bind();

        // Fill the VBO and the EBO with the vertices and the indices data
        this->vbo.set_data(vertex_count * sizeof(Vertex), vertices, usage);
        this->ebo.set_data(index_count * sizeof(unsigned int), indices, usage);
        
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
    void draw(const GLenum drawing_mode = GL_TRIANGLES) const
    {
        this->vao.bind();
        glDrawElements(drawing_mode, this->index_count, GL_UNSIGNED_INT, nullptr);
        debug_record_draw_call(static_cast<uint32_t>(this->index_count));
        this->vao.unbind();
    }

    int get_index_count() const {
        return this->index_count;
    }
    bool operator<(const Mesh& other) const
    {
        return this->index_count < other.index_count;
    }
};

#endif //ENGINE_MESH_HPP