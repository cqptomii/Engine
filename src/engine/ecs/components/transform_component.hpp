/**
 * @file transform_component.hpp
 * @author FRAISSE Tom
 * @brief 
 * @version 0.1
 * @date 2026-03-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glm/gtx/quaternion.hpp"
/**
 * @brief Transform component class
 * 
 */
class TransformComponent
{
    /**
     * @brief Position of the transform
     * 
     */
    glm::vec3 transform_position;
    /**
     * @brief Rotation of the transform
     * 
     */
    glm::quat transform_rotation;
    /**
     * @brief Scale of the transform
     * 
     */
    glm::vec3 transform_scale;

public:
    /**
     * @brief Constructor of the Transform Component
     * 
     */
    TransformComponent() : transform_position(glm::vec3(0.0f)), transform_rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), transform_scale(glm::vec3(1.0f)) {}
    
    /**
     * @brief Constructor of the Transform Component
     * 
     * @param position : Position of the transform
     * @param rotation : Rotation of the transform
     * @param scale : Scale of the transform
     */
    TransformComponent( const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale ) : transform_position(position), transform_rotation(rotation), transform_scale(scale)
    {
        // Guard against a degenerate (e.g. value-initialized zero) quaternion:
        // multiplying by a zero quaternion stays zero and silently disables all
        // rotation. Fall back to identity when the input is not a valid rotation.
        if (glm::dot(this->transform_rotation, this->transform_rotation) < 1e-8f)
        {
            this->transform_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            this->transform_rotation = glm::normalize(this->transform_rotation);
        }
    }
    
    /**
     * @brief Destructor of the Transform Component
     * 
     */
    ~TransformComponent() = default;

    /**
     * @brief Get the position of the transform
     * 
     * @return glm::vec3 : Position of the transform
     */
    glm::vec3 get_position() const noexcept
    {
        return this->transform_position;
    }
    /**
     * @brief Get the rotation of the transform
     * 
     * @return glm::quat : Rotation of the transform
     */
    glm::quat get_rotation() const noexcept
    {
        return this->transform_rotation;
    }
    /**
     * @brief Get the scale of the transform
     * 
     * @return glm::vec3 : Scale of the transform
     */
    glm::vec3 get_scale() const noexcept
    {
        return this->transform_scale;
    }
    
    /**
     * @brief Set the position of the transform
     * 
     * @param position : Position of the transform
     */
    void set_position(const glm::vec3& position) noexcept{
        this->transform_position = position;
    }

    /**
     * @brief Set the rotation of the transform
     * 
     * @param rotation : Rotation of the transform
     */
    void set_rotation(const glm::quat& rotation) noexcept{
        this->transform_rotation = rotation;
    }

    /**
     * @brief Set the scale of the transform
     * 
     * @param scale : Scale of the transform
     */
    void set_scale(const glm::vec3& scale) noexcept{
        this->transform_scale = scale;
    }

    /**
     * @brief Translate the transform
     * 
     * @param translation : Translation of the transform
     */
    void translate(const glm::vec3& translation) noexcept{
        this->transform_position += translation;
    }
    /**
     * @brief Rotate the transform
     * 
     * @param rotation : Rotation of the transform
     */
    void rotate(const glm::quat& rotation) noexcept{
        this->transform_rotation = glm::normalize(rotation * this->transform_rotation);
    }

    /**
     * @brief Scale the transform uniformly
     * 
     * @param scale : Scale of the transform
     */
    void scale_uniform(const float factor) noexcept{
        this->transform_scale *= factor;
    }
    /**
     * @brief Scale the transform non-uniformly
     * 
     * @param scale : Scale of the transform
     */
    void scale(const glm::vec3& scale) noexcept{
        this->transform_scale *= scale;
    }


    /**
     * @brief Get the model matrix of the transform
     * 
     * @return glm::mat4 : Model matrix of the transform
     */
    glm::mat4 get_model_matrix() const noexcept
    {
        return glm::translate(glm::mat4(1.0f), this->transform_position) * glm::toMat4(this->transform_rotation) * glm::scale(glm::mat4(1.0f), this->transform_scale);
    }

    /**
     * @brief Equality operator
     * 
     * @param other : Other transform component
     * @return bool : True if the transform components are equal, false otherwise
     */
    bool operator==(const TransformComponent& other) const
    {
        return this->transform_position == other.transform_position && this->transform_rotation == other.transform_rotation && this->transform_scale == other.transform_scale;
    }

    /**
     * @brief Inequality operator
     * 
     * @param other : Other transform component
     * @return bool : True if the transform components are not equal, false otherwise
     */
    bool operator!=(const TransformComponent& other) const
    {
        return this->transform_position != other.transform_position || this->transform_rotation != other.transform_rotation || this->transform_scale != other.transform_scale;
    }
};

#endif //TRANSFORM_COMPONENT_HPP
