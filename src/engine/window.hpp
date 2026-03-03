//
// Created by tomfr on 03/03/2026.
//

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>
#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum class WindowType
{
    EDITOR,
    RUN
};

class Window
{
private:
    GLFWwindow* window_ptr = nullptr;
    int width, height;
    double cursor_x, cursor_y;

    std::string window_title;
    WindowType window_type;

    void init()
    {
        // Create GLFW Window
        this->window_ptr = glfwCreateWindow(this->width, this->height, this->window_title.c_str(), nullptr, nullptr);
        if (!this->window_ptr)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            exit(EXIT_FAILURE);
        }

        glfwSetWindowUserPointer(this->window_ptr, this);
        glfwMakeContextCurrent(this->window_ptr);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            exit(EXIT_FAILURE);
        }

        glViewport(0,0, this->width, this->height);

        // Set Callback functions
        glfwSetWindowSizeCallback(this->window_ptr, window_size_callback);
        glfwSetCursorPosCallback(this->window_ptr, mouse_pos_callback);
        glfwSetScrollCallback(this->window_ptr, scroll_callback);

        std::cout << "Window created" << std::endl;
        std::cout << "Window size: " << this->width << "x" << this->height << std::endl;

        std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    }

public:
    Window() : width(800), height(600), cursor_x(0), cursor_y(0), window_title("Untitled"), window_type(WindowType::EDITOR)
    {
        this->init();
    }
    explicit Window(const int width = 800, const int height = 600, std::string title = "Untitled", const WindowType type) : width(width), height(height), window_title(std::move(title)), window_type(type), cursor_x(0), cursor_y(0)
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

    //
    // Callback functions
    //

    static void window_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
    static void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
    {

    }
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {

    }

    // Function to update the window buffers and poll events
    void update() const
    {
        // Swap window buffer
        glfwSwapBuffers(this->window_ptr);
        // Poll for and process events
        glfwPollEvents();
    }
};

#endif //WINDOW_HPP
