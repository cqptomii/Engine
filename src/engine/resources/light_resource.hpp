//
// Created by tomfr on 12/03/2026.
//

#ifndef ENGINE_LIGHT_RESOURCE_HPP
#define ENGINE_LIGHT_RESOURCE_HPP

#include <glm/glm.hpp>

class LightResource
{
    glm::vec3 position;
    glm::vec3 color;

    float ambient_strength = 0.1f;
    float diffuse_strength = 0.5f;
    float specular_strength = 0.5f;
public:
    LightResource(const glm::vec3 position, const glm::vec3 color) : position(position), color(color) {}

    glm::vec3 get_position() const noexcept
    {
        return this->position;
    }
    glm::vec3 get_color() const noexcept
    {
        return this->color;
    }
};

#endif //ENGINE_LIGHT_RESOURCE_HPP