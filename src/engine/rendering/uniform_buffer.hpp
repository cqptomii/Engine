#ifndef UNIFORM_BUFFER_HPP
#define UNIFORM_BUFFER_HPP
#include "glad/glad.h"

// UniformBuffer class to manage OpenGL uniform buffers
class UniformBuffer
{
    GLuint buffer_slot;
    GLuint buffer_id;
    size_t size;
public:
    UniformBuffer(const size_t size, const GLuint binding_slot) : buffer_slot(binding_slot), size(size)
    {
        glGenBuffers(1, &buffer_id);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer_id);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, buffer_slot, buffer_id);
    }
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    ~UniformBuffer()
    {
        // Unbind the buffer before deletion
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glDeleteBuffers(1, &buffer_id);
    }
    void update(const void* data) const
    {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer_id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
    }
};

#endif //ENGINE_UNIFORM_BUFFER_HPP
