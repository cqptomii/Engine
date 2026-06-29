//
// Created by tomfr on 03/03/2026.
//

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>
#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "input/input_manager.hpp"
#include "engine/systems/input_system.hpp"

class Window
{
    GLFWwindow* window_ptr = nullptr;

    std::string window_title;
    InputSystem input_system;

    void init()
    {
        // Create GLFW Window
        this->window_ptr = glfwCreateWindow(this->width, this->height, this->window_title.c_str(), nullptr, nullptr);
        if (!this->window_ptr)
        {
            const char* error_desc = nullptr;
            const int error_code = glfwGetError(&error_desc);
            std::cerr << "Failed to create GLFW window with OpenGL 3.3 core"
                      << " (error " << error_code << ": "
                      << (error_desc ? error_desc : "unknown") << ")"
                      << std::endl;

            // Fallback: relax profile/version requirements for older drivers.
            glfwDefaultWindowHints();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

            this->window_ptr = glfwCreateWindow(this->width, this->height, this->window_title.c_str(), nullptr, nullptr);
            if (!this->window_ptr)
            {
                const char* fallback_desc = nullptr;
                const int fallback_code = glfwGetError(&fallback_desc);
                std::cerr << "Fallback context creation also failed"
                          << " (error " << fallback_code << ": "
                          << (fallback_desc ? fallback_desc : "unknown") << ")"
                          << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        glfwSetWindowUserPointer(this->window_ptr, this);
        glfwMakeContextCurrent(this->window_ptr);
        
        // By default VSync is enabled
        set_VSync(1);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Enable Blending
        enable_blending();

        glViewport(0,0, this->width, this->height);

        // Set Callback functions
        glfwSetWindowSizeCallback(this->window_ptr, window_size_callback);
        glfwSetCursorPosCallback(this->window_ptr, mouse_pos_callback);
        glfwSetScrollCallback(this->window_ptr, scroll_callback);
        glfwSetMouseButtonCallback(this->window_ptr, mouse_button_callback);
        glfwSetKeyCallback(this->window_ptr, key_callback);


        std::cout << "Window created" << std::endl;
        std::cout << "Window size: " << this->width << "x" << this->height << std::endl;

        std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    }

public:
    int width, height;


    Window() : window_title("Untitled"), width(800), height(600)
    {
        this->init();
    }
    explicit Window(const int width = 800, const int height = 600, std::string title = "Untitled") : window_title(std::move(title)), width(width), height(height)
    {
        this->init();
    }
    ~Window()
    {
        if (window_ptr != nullptr)
        {
            glfwDestroyWindow(window_ptr);
        }
    }
    bool isOpen() const
    {
        if (glfwWindowShouldClose(this->window_ptr))
        {
            return false;
        }
        return true;
    }
    void closeWindow() const
    {
        glfwSetWindowShouldClose(this->window_ptr, true);
    }

    //
    // Function to interact with the window by controllers
    //

    bool is_key_pressed(const int key) const
    {
        return glfwGetKey(this->window_ptr, key) == GLFW_PRESS;
    }
    bool is_key_released(const int key) const
    {
        return glfwGetKey(this->window_ptr, key) == GLFW_RELEASE;
    }
    bool is_mouse_button_pressed(const int button) const
    {
        return glfwGetMouseButton(this->window_ptr, button) == GLFW_PRESS;
    }


    GLFWwindow* get_window_ptr() const noexcept
    {
        return this->window_ptr;
    }
    InputSystem& get_input_system()
    {
        return this->input_system;
    }
    
    //
    // Callback functions
    //
    void setUserPointer(void* user_ptr) const
    {
        glfwSetWindowUserPointer(this->window_ptr, user_ptr);
    }
    static void window_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
    static void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
    {
        auto* input_system = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
        input_system->on_mouse_move(xpos, ypos);
    }
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        auto* input_system = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
        input_system->on_mouse_button_event(button, action, mods);
    }
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto* input_system = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
        input_system->on_mouse_scroll(xoffset, yoffset);
    }
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto* input_system = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
        input_system->on_key_event(key, scancode, action, mods);
    }

    // Function to update the window buffers and poll events
    void update() const
    {
        // Swap window buffer
        glfwSwapBuffers(this->window_ptr);
        // Poll for and process events
        glfwPollEvents();
    }


    static void set_VSync(const int interval)
    {
        glfwSwapInterval(interval);
    }
    static void set_depth_test(const bool enable)
    {
        if (enable)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }
    static void disable_blending()
    {
        glDisable(GL_BLEND);
    }
    static void enable_blending()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
};

#endif //WINDOW_HPP
