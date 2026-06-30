/**
 * @file editor_camera.hpp
 * @author Tom FRAISSE
 * @brief Class to manage the camera in the editor
 * @version 0.1
 * @date 2026-03-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef EDITOR_CAMERA_HPP
#define EDITOR_CAMERA_HPP

#include "glm/ext/vector_float3.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glad/glad.h>
#include "engine/rendering/utils/camera_data.hpp"
#include "engine/core/camera_movement.hpp"


class EditorCamera
{
    // Camera parameters
    float near_plane = 0.1f;
    float far_plane = 500.0f;
    float aspect_ratio = 16./9.;

    // Camera movement parameters
    float cam_rotate_sensitivity = 0.25f;
    float cam_move_sensitivity    = 0.002f; 
    float cam_zoom_sensitivity   = 0.1f;

    // Field of view config
    float cam_fov = 45.0f;

    // Orientation Angle configuration
    float cam_yaw;
    float cam_pitch;

    // Camera center point
    glm::vec3 cam_target = glm::vec3(0.0f, 0.0f, 0.0f);
    float cam_distance = 5.0f;

    // Camera initial position 
    glm::vec3 cam_initial_position = glm::vec3(0.0f, 1.0f, 0.0f);

    // Camera position and direction
    glm::vec3 cam_position = this->cam_initial_position;
    glm::vec3 cam_direction = glm::normalize(this->cam_initial_position - this->cam_target);
    glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cam_right = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 v_up = glm::vec3(0.0f, 1.0f, 0.0f);

    /**
     * @brief Method to update the camera parameters
     * 
     */
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

    /**
     * @brief Constructor of the EditorCamera class
     * @details Constructor of the EditorCamera class
     * @param position Position of the camera
     * @param direction Direction of the camera
     */
    EditorCamera() : cam_yaw(-45.), cam_pitch(0.), cam_up(), cam_right()
    {
        this->update_cam_parameters();
    }
    /**
     * @brief Constructor of the EditorCamera class
     * @details Constructor of the EditorCamera class
     * @param position Position of the camera
     * @param direction Direction of the camera
     */
    EditorCamera(glm::vec3 initial_position, glm::vec3 direction) : cam_initial_position(initial_position), cam_position(initial_position), cam_direction(direction), cam_yaw(-45.), cam_pitch(0.), cam_up(), cam_right()
    {
        this->update_cam_parameters();
    }

    /**
     * @brief Destructor of the EditorCamera class
     */
    ~EditorCamera() = default;

    /**
     * @brief Method to get the view matrix
     * @details The view matrix is the matrix that transforms the world coordinates to the camera coordinates
     * @return glm::mat4 View matrix
     */
    glm::mat4 get_view_matrix() const noexcept
    {
        return glm::lookAt(this->cam_position, this->cam_position + this->cam_direction, this->cam_up);
    }

    /**
     * @brief Method to get the projection matrix
     * @details The projection matrix is the matrix that transforms the camera coordinates to the normalized device coordinates
     * @param w_width Width of the window
     * @param w_height Height of the window
     * @return glm::mat4 Projection matrix
     */
    glm::mat4 get_projection_matrix(const int w_width, const int w_height) noexcept
    {
        this->aspect_ratio = static_cast<float>(w_width) / static_cast<float>(w_height);
        return glm::perspective(glm::radians(this->cam_fov), this->aspect_ratio, this->near_plane, this->far_plane);
    }

    /**
     * @brief Method to get the camera position
     * @details The camera position is the position of the camera in the world coordinates
     * @return glm::vec3 Camera position
     */
    glm::vec3 get_position() const noexcept
    {
        return this->cam_position;
    }

    /**
     * @brief Method to get the camera direction
     * @details The camera direction is the direction of the camera in the world coordinates
     * @return glm::vec3 Camera direction
     */
    glm::vec3 get_direction() const noexcept
    {
        return this->cam_direction;
    }

    /**
     * @brief Method to get the camera data
     * @details The camera data is the data of the camera in the world coordinates
     * @param width Width of the window
     * @param height Height of the window
     * @return CameraData Camera data
     */
    CameraData get_camera_data(const int width, const int height)
    {
        return {
            this->get_view_matrix(),
            this->get_projection_matrix(width, height),
            this->cam_position,
        };
    }

    /**
     * @brief Method to convert a screen point to a ray
     * @details The method converts a screen point to a ray in the world coordinates
     * @param mouse_x X coordinate of the mouse
     * @param mouse_y Y coordinate of the mouse
     * @param screen_width Width of the screen
     * @param screen_height Height of the screen
     * @param ray_origin Origin of the ray
     * @param ray_direction Direction of the ray
     */
    void screen_point_to_ray(const float mouse_x, const float mouse_y, const int screen_width, const int screen_height, glm::vec3& ray_origin, glm::vec3& ray_direction)
    {
        // Convert screen coordinates to normalized device coordinates (NDC)
        float x = (2.0f * mouse_x) / screen_width - 1.0f;
        float y = 1.0f - (2.0f * mouse_y) / screen_height;
        float z = 1.0f;

        // Create a ray in NDC space
        glm::vec4 ray_nds(x, y, z, 1.0f);

        // Convert the ray from NDC to world space
        glm::mat4 inv_projection = glm::inverse(this->get_projection_matrix(screen_width, screen_height));
        glm::mat4 inv_view = glm::inverse(this->get_view_matrix());
        glm::vec4 ray_world = inv_view * inv_projection * ray_nds;

        ray_direction = glm::normalize(glm::vec3(ray_world));
        ray_origin = this->cam_position;
    }

    /**
     * @brief Method to process the camera movement
     * @details The method processes the camera movement in the world coordinates
     * @param direction Direction of the movement
     * @param delta_time Delta time
     */
    void process_cam_movement(const CameraMovement direction, const float delta_time)
    {
        const float speed = this->cam_move_sensitivity * delta_time;

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
        else if (direction == TOP) {
            this->cam_target += this->cam_up * speed;
        }
        else if(direction == BOTTOM){
            this->cam_target -= this->cam_up * speed;
        }

        this->update_cam_parameters();
    }

    /**
     * @brief Method to process the camera movement with the movement vector in the screen space
     * @details The method processes the camera movement in the world coordinates
     * @param direction (glm::vec3) : Movement vector in screen space
     */
    void process_cam_movement(float dx, float dy)
    {
        // Move Speed proportional to the target distance
        const float pan_speed = this->cam_distance * this->cam_move_sensitivity;

        this->cam_target += (-this->cam_right * dx + this->cam_up * dy) * pan_speed;
        
        this->update_cam_parameters();
    }
    
    /**
     * @brief Method to process the camera rotation
     * @details The method processes the camera rotation in the world coordinates
     * @param x_offset X offset of the rotation
     * @param y_offset Y offset of the rotation
     * @param z_offset Z offset of the rotation
     * @param constrainPitch If true, the pitch will be constrained between -89 and 89 degrees
     */
    void process_cam_rotation(const float x_offset, const float y_offset, const float z_offset, GLboolean constrainPitch = true)
    {
        this->cam_yaw += x_offset * this->cam_rotate_sensitivity;
        this->cam_pitch += y_offset * this->cam_rotate_sensitivity;

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

    /**
     * @brief Method to process the camera zoom
     * @details The method processes the camera zoom in the world coordinates
     * @param y_offset Y offset of the zoom
     */
    void process_cam_zoom(const float y_offset)
    {
        // Update the distance from the target
        this->cam_distance *= (1.0f - y_offset * this->cam_zoom_sensitivity);

        // Clamp the distance
        this->cam_distance = glm::clamp(this->cam_distance, 0.05f, 100.0f);

        this->update_cam_parameters();
    }

    /**
     * @brief Method to reset the camera to his original position on the screen
     * @details The method resets the camera to his original position on the screen
     */
    void reset(){

        // Reset the target point
        this->cam_target   = glm::vec3(0.0f);
        
        // Reset Orientation
        this->cam_yaw      = -45.0f;
        this->cam_pitch    = 0.0f;

        // Reset the cam distance
        this->cam_distance = 5.0f;

        // Recalculate the cam parameters
        this->update_cam_parameters();
    }

    /**
     * @brief Method to debug the camera
     * @details The method debugs the camera in the console
     */
    void debug_cam()
    {
        // Show cam Position
        std::cout << "Editor Camera Position: " << cam_position[0] << " " << cam_position[1] << " " << cam_position[2] << std::endl;
        std::cout << "Editor Camera Direction: " << cam_direction[0] << " " << cam_direction[1] << " " << cam_direction[2] << std::endl;
    }

};


#endif //EDITOR_CAMERA_HPP
