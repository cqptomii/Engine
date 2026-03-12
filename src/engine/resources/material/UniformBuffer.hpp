//
// Created by tomfr on 13/03/2026.
//

#ifndef ENGINE_UNIFORMBUFFER_HPP
#define ENGINE_UNIFORMBUFFER_HPP

#include <glm/glm.hpp>
#include "glad/glad.h"

struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 projection;
};
struct ObjectUBO
{
    glm::mat4 model;
};
struct MaterialUBO
{
    glm::vec4 albedo;
    float metallic;
    float roughness;
};


class UniformBuffer
{
    GLuint buffer;
    size_t size;
public:
    UniformBuffer(const size_t size, const GLuint binding) : buffer(binding), size(size)
    {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer);
    }
    void update(const void* data) const
    {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
    }
};

#endif //ENGINE_UNIFORMBUFFER_HPP