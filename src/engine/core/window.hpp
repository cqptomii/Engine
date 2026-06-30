//
// Created by tomfr on 03/03/2026.
//

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>
#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/systems/input_system.hpp"

class Window
{
    GLFWwindow* window_ptr = nullptr;

    std::string window_title;
    
    /**
     * @brief 
     * 
     */
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

    // Window size parameters
    int width, height;

    /**
     * @brief Default Window Constructor
     * 
     */
    Window() : window_title("Untitled"), width(800), height(600)
    {
        this->init();
    }

    /**
     * @brief Override Window Constructor
     * 
     * @param width  (int): Width size of the window
     * @param height (int): Height size of the window
     * @param title (str): Name of the window
     */
    explicit Window(const int width = 800, const int height = 600, std::string title = "Untitled") : window_title(std::move(title)), width(width), height(height)
    {
        this->init();
    }

    /**
     * @brief Destructor of the class
     *  
     */
    ~Window()
    {
        if (window_ptr != nullptr)
        {
            glfwDestroyWindow(window_ptr);
        }
    }

    /**
     * @brief Method that check if the window is still open 
     * 
     * @return true : The window is open
     * @return false : The window is closed
     */
    bool isOpen() const
    {
        if (glfwWindowShouldClose(this->window_ptr))
        {
            return false;
        }
        return true;
    }

    /**
     * @brief Method to close the window
     * 
     */
    void closeWindow() const
    {
        glfwSetWindowShouldClose(this->window_ptr, true);
    }

    //
    // Function to interact with the window by controllers
    //

    /**
     * @brief  Method which returned the pressed state of a given key
     * 
     * @param key : KeyCode of the key 
     * @return true : The key is pressed
     * @return false : the key isn't pressed
     */
    bool is_key_pressed(const int key) const
    {
        return glfwGetKey(this->window_ptr, key) == GLFW_PRESS;
    }

    /**
     * @brief  Method which returned the released state of a given key
     * 
     * @param key : Keycode of the key
     * @return true : The key is released
     * @return false : The key isn't released
     */
    bool is_key_released(const int key) const
    {
        return glfwGetKey(this->window_ptr, key) == GLFW_RELEASE;
    }
    
    /**
     * @brief Method which check if a given mousebutton is pressed
     * 
     * @param button : Code of the button 
     * @return true : The mouse button is pressed
     * @return false : The mouse button isn't pressed
     */
    bool is_mouse_button_pressed(const int button) const
    {
        return glfwGetMouseButton(this->window_ptr, button) == GLFW_PRESS;
    }


    /**
     * @brief Get the window ptr object
     * 
     * @return GLFWwindow* : Window pointer object
     */
    GLFWwindow* get_window_ptr() const noexcept
    {
        return this->window_ptr;
    }
    
    //
    // Callback functions
    //

    /**
     * @brief Method to set the user pointer of the window
     * 
     * @param user_ptr : Pointer to the user data
     */
    void setUserPointer(void* user_ptr) const
    {
        glfwSetWindowUserPointer(this->window_ptr, user_ptr);
    }


    /**
     * @brief Method to set the window size
     * 
     * @param window : Window pointer
     * @param width : Width of the window
     * @param height : Height of the window
     */
    static void window_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    /**
     * @brief Method to set the mouse position
     * 
     * @param window : Window pointer
     * @param xpos : X position of the mouse
     * @param ypos : Y position of the mouse
     */
    static void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
    {
        auto* input_system = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
        input_system->on_mouse_move(xpos, ypos);
    }

    /**
     * @brief Method to set the mouse button
     * 
     * @param window : Window pointer
     * @param button : Button code
     * @param action : Action code
     * @param mods : Modifier code
     */
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        auto* input_system = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
        input_system->on_mouse_button_event(button, action, mods);
    }

    /**
     * @brief Method to set the scroll
     * 
     * @param window : Window pointer
     * @param xoffset : X offset
     * @param yoffset : Y offset
     */
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto* input_system = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
        input_system->on_mouse_scroll(xoffset, yoffset);
    }

    /**
     * @brief Method to set the key
     * 
     * @param window : Window pointer
     * @param key : Key code
     * @param scancode : Scancode
     * @param action : Action code
     * @param mods : Modifier code
     */
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto* input_system = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
        input_system->on_key_event(key, scancode, action, mods);
    }

    /**
     * @brief Method which update the buffers of the window and listen events from the controllers
     * 
     */
    void update() const
    {
        // Swap window buffer
        glfwSwapBuffers(this->window_ptr);
        // Poll for and process events
        glfwPollEvents();
    }


    /**
     * @brief Method to set the VSync
     * 
     * @param interval : Interval of the VSync
     */
    void set_VSync(const int interval) const
    {
        glfwSwapInterval(interval);
    }
    
    /**
     * @brief Set the depth test in OPENGL
     * 
     * @param enable (bool)
    */
    void set_depth_test(const bool enable) const
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

    /**
     * @brief Method to disable alpha Blending
     * 
     */
    void disable_blending() const
    {
        glDisable(GL_BLEND);
    }

    /**
     * @brief Method to enable alpha blending
     * 
     */
    void enable_blending() const
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
};

#endif //WINDOW_HPP
