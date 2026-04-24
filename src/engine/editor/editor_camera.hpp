//
// Created by tomfr on 05/03/2026.
//

#ifndef EDITOR_CAMERA_HPP
#define EDITOR_CAMERA_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glad/glad.h>
#include "engine/rendering/utils/camera_data.hpp"

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
    float far_plane = 500.0f;
    float aspect_ratio = 16./9.;
    bool first_move = false;

    float cam_sensitivity = 0.1f;
    float cam_velocity = 2.5f;
    float cam_fov = 45.0f;
    float cam_yaw;
    float cam_pitch;
    float cam_roll;

    glm::vec3 cam_target = glm::vec3(0.0f, 0.0f, 0.0f);
    float cam_distance = 5.0f;

    glm::vec3 cam_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cam_direction = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cam_up;
    glm::vec3 cam_right;

    glm::vec3 v_up = glm::vec3(0.0f, 1.0f, 0.0f);

    void update_cam_parameters()
    {
        // Update the orbital camera
        this->cam_position.x = this->cam_target.x + cam_distance * glm::cos(glm::radians(this->cam_pitch)) * glm::sin(glm::radians(this->cam_yaw));
        this->cam_position.y = this->cam_target.y + cam_distance * glm::sin(glm::radians(this->cam_pitch));
        this->cam_position.z = this->cam_target.z + cam_distance * glm::cos(glm::radians(this->cam_pitch)) * glm::cos(glm::radians(this->cam_yaw));

        this->cam_direction = glm::normalize(this->cam_target - this->cam_position);

        cam_right = glm::normalize(glm::cross(cam_direction, v_up));
        cam_up = glm::normalize(glm::cross(cam_right, cam_direction));
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
    glm::mat4 get_projection_matrix(const int w_width, const int w_height) noexcept
    {
        this->aspect_ratio = static_cast<float>(w_width) / static_cast<float>(w_height);
        return glm::perspective(glm::radians(this->cam_fov), this->aspect_ratio, this->near_plane, this->far_plane);
    }
    glm::vec3 get_position() const noexcept
    {
        return this->cam_position;
    }
    glm::vec3 get_direction() const noexcept
    {
        return this->cam_direction;
    }
    CameraData get_camera_data(const int width, const int height)
    {
        return {
            this->get_view_matrix(),
            this->get_projection_matrix(width, height),
            this->cam_position,
        };
    }

    void process_cam_movement(const CameraMovement direction, const float delta_time)
    {
        const float speed = this->cam_sensitivity * delta_time;

        const glm::vec3 forward = glm::normalize(this->cam_direction);

        if (direction == BACKWARD)
        {
            this->cam_target -= forward * speed;
        }else if (direction == FORWARD)
        {
            this->cam_target += forward * speed;
        }else if (direction == LEFT)
        {
            this->cam_target -= this->cam_right * speed;
        }
        else if (direction == RIGHT)
        {
            this->cam_target += this->cam_right * speed;
        }

        this->update_cam_parameters();
    }
    void process_cam_movement(const glm::vec3& direction)
    {
        this->cam_target += direction * this->cam_sensitivity;

        this->update_cam_parameters();
    }
    void process_cam_rotation(const float x_offset, const float y_offset, const float z_offset, GLboolean constrainPitch = true)
    {
        this->cam_yaw += x_offset * this->cam_sensitivity;
        this->cam_pitch += y_offset * this->cam_sensitivity;
        this->cam_roll += z_offset * this->cam_sensitivity;

        if (constrainPitch)
        {
            if (this->cam_pitch > 89.)
            {
                this->cam_pitch = 89.;
            } else if (this->cam_pitch < -89.)
            {
                this->cam_pitch = -89.;
            }
        }

        // Update camera vectors
        this->update_cam_parameters();
    }
    void process_cam_zoom(const float y_offset)
    {
        cam_distance -= y_offset * 5*this->cam_sensitivity;
        if (this->cam_distance < 1.0f) this->cam_distance = 1.0f;
        if (cam_distance > 100.0f) cam_distance = 100.0f;

        this->update_cam_parameters();
    }

    void debug_cam()
    {
        // Show cam Position
        std::cout << "Editor Camera Position: " << cam_position[0] << " " << cam_position[1] << " " << cam_position[2] << std::endl;
        std::cout << "Editor Camera Direction: " << cam_direction[0] << " " << cam_direction[1] << " " << cam_direction[2] << std::endl;
    }

};


#endif //EDITOR_CAMERA_HPP
