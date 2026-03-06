//
// Created by tomfr on 06/03/2026.
//

#ifndef EDITOR_VIEWPORT_HPP
#define EDITOR_VIEWPORT_HPP


#include <memory>
#include <GLFW/glfw3.h>
#include "editor_camera.hpp"
#include "../core/window.hpp"



class EditorViewport{

    std::unique_ptr<Window> editor_window;
    EditorCamera editor_camera;

    bool first_movement = false;

    // Mouse last position, by default to 0.0f
    float mouse_last_x = 0.0f;
    float mouse_last_y = 0.0f;

    void init_viewport()
    {
        GLFWwindow* win = this->editor_window->get_window_ptr();

        glfwSetWindowUserPointer(win, this);

        // Set mouse pos callback function
        glfwSetCursorPosCallback(win, [](GLFWwindow* window, double x_pos, double y_pos)
        {
            const auto viewport = static_cast<EditorViewport*>(glfwGetWindowUserPointer(window));
            viewport->on_mouse_move(x_pos, y_pos);
        });

        // Set the mouse scroll callback function
        glfwSetScrollCallback(win, [](GLFWwindow* window, double x_offset, double y_offset)
        {
            const auto viewport = static_cast<EditorViewport*>(glfwGetWindowUserPointer(window));
            viewport->on_mouse_scroll(x_offset, y_offset);
        });
    }

public:

    EditorViewport() : editor_window(std::make_unique<Window>()), editor_camera(EditorCamera())
    {
        this->init_viewport();
    }
    EditorViewport( std::unique_ptr<Window> window, EditorCamera camera) : editor_window(std::move(window)), editor_camera(camera)
    {
        this->init_viewport();
    }
    ~EditorViewport() = default;

    void on_mouse_move(double x_in, double y_in)
    {
        const auto x_pos = static_cast<float>(x_in);
        const auto y_pos = static_cast<float>(y_in);

        // Calculate the offset between two positions
        if (this->first_movement)
        {
            this->mouse_last_x = x_pos;
            this->mouse_last_y = y_pos;
            this->first_movement = false;
        }

        const float x_offset = x_pos - this->mouse_last_x;
        const float y_offset = y_pos - this->mouse_last_y;

        this->mouse_last_x = x_pos;
        this->mouse_last_y = y_pos;

        // Update the camera rotation
        this->editor_camera.process_cam_rotation(
            x_offset,
            y_offset,
            0
        );
    }
    void on_mouse_scroll(double x_offset, double y_offset)
    {
        // Update the fov of the camera
        this->editor_camera.process_cam_zoom(static_cast<float>(y_offset));
    }
};



#endif //EDITOR_VIEWPORT_HPP
