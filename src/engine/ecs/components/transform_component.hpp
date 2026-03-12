//
// Created by tomfr on 03/03/2026.
//

#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glm/gtx/quaternion.hpp"

class TransformComponent
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

public:
    TransformComponent() : position(glm::vec3(0.0f)), rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), scale(glm::vec3(1.0f)) {}
    TransformComponent( const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale ) : position(position), rotation(rotation), scale(scale) {}
    ~TransformComponent() = default;

    glm::vec3 get_position() const noexcept
    {
        return this->position;
    }
    glm::quat get_rotation() const noexcept
    {
        return this->rotation;
    }
    glm::vec3 get_scale() const noexcept
    {
        return this->scale;
    }
    glm::mat4 get_model_matrix() const noexcept
    {
        return glm::translate(glm::mat4(1.0f), this->position) * glm::toMat4(this->rotation) * glm::scale(glm::mat4(1.0f), this->scale);
    }

    bool operator==(const TransformComponent& other) const
    {
        return this->position == other.position && this->rotation == other.rotation && this->scale == other.scale;
    }
    bool operator!=(const TransformComponent& other) const
    {
        return this->position != other.position || this->rotation != other.rotation || this->scale != other.scale;
    }
};

#endif //TRANSFORM_COMPONENT_HPP
