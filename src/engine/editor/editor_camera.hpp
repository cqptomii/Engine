//
// Created by tomfr on 05/03/2026.
//

#ifndef EDITOR_CAMERA_HPP
#define EDITOR_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glad/glad.h"

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

class EditorCamera
{
    float near_plane = 0.1f;
    float far_plane = 1000.0f;
    float aspect_ratio = 16./9.;
    bool first_move = false;

    float cam_sensitivity = 0.5f;
    float cam_velocity = 2.5f;
    float cam_fov = 45.0f;
    float cam_yaw;
    float cam_pitch;
    float cam_roll;

    glm::vec3 cam_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cam_direction = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cam_up;
    glm::vec3 cam_right;

    glm::vec3 v_up = glm::vec3(0.0f, 1.0f, 0.0f);

    void update_cam_parameters()
    {
        // Update the cam direction by using quaternion

        const glm::quat rotation = glm::quat(glm::vec3(
            glm::radians(cam_yaw),
            glm::radians(cam_pitch),
            glm::radians(cam_roll)
            ));


        // Update cam vectors
        auto direction = glm::vec3(1.0f);
        cam_direction = glm::normalize(rotation * direction);

        cam_right = glm::normalize(glm::cross(v_up, cam_direction));
        cam_up = glm::normalize(glm::cross(cam_direction, cam_right));
    }
public:
    EditorCamera() : cam_yaw(-90.), cam_pitch(0.), cam_roll(0.), cam_up(), cam_right()
    {
        this->update_cam_parameters();
    }

    EditorCamera(glm::vec3 position, glm::vec3 direction) : cam_position(position), cam_direction(direction), cam_yaw(-90.), cam_pitch(0.), cam_roll(0.), cam_up(), cam_right()
    {
        this->update_cam_parameters();
    }
    ~EditorCamera() = default;

    glm::mat4 get_view_matrix() const noexcept
    {
        return glm::lookAt(this->cam_position, this->cam_position + this->cam_direction, this->cam_up);
    }
    glm::mat4 get_projection_matrix() const noexcept
    {
        return glm::perspective(glm::radians(this->cam_fov), this->aspect_ratio, this->near_plane, this->far_plane);
    }

    void process_cam_movement(CameraMovement direction, float delta_time)
    {
        if (!this->first_move)
        {
            this->first_move = true;
        }

        const float speed = this->cam_velocity * delta_time;

        const glm::vec3 forward = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));

        if (direction == BACKWARD)
        {
            this->cam_position -= forward * speed;
        }else if (direction == FORWARD)
        {
            this->cam_position += forward * speed;
        }else if (direction == LEFT)
        {
            this->cam_position -= this->cam_right * speed;
        }
        else if (direction == RIGHT)
        {
            this->cam_position += this->cam_right * speed;
        }
    }
    void process_cam_rotation(const float x_offset, const float y_offset, const float z_offset)
    {
        this->cam_yaw += x_offset;
        this->cam_pitch += y_offset;
        this->cam_roll += z_offset;

        if (this->cam_pitch > 90.)
        {
            this->cam_pitch = 90.;
        } else if (this->cam_pitch < -90.)
        {
            this->cam_pitch = -90.;
        }
    }
    void process_cam_zoom(const float y_offset)
    {
        this->cam_fov -= y_offset;
        if (this->cam_fov < 1.0f) this->cam_fov = 1.0f;
        if (this->cam_fov > 55.0f) this->cam_fov = 55.0f;
    }
};


#endif //EDITOR_CAMERA_HPP
