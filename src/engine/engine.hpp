//
// Created by tomfr on 03/03/2026.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <chrono>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../src/engine/window.hpp"

class Engine
{
    std::unique_ptr<Window> editor_window_ptr = nullptr;
    std::unique_ptr<Window> run_window_ptr = nullptr;
    // Scene

    std::atomic<bool> is_running;
    bool is_paused;
    double fixed_timestep{1.0/60.0};

    static void init()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    void cleanup()
    {
        delete editor_window_ptr;
        delete run_window_ptr;
    }
    void process_input() const
    {
        if (this->editor_window_ptr->is_key_pressed(GLFW_KEY_ESCAPE))
        {
            this->editor_window_ptr->closeWindow();
        }
    }

public:
    Engine() : is_running(false), is_paused(false)
    {
        init();
        editor_window_ptr = std::make_unique<Window>(Window(800, 600, "Engine", WindowType::EDITOR));
    }
    explicit Engine(std::unique_ptr<Window> editor_window) : is_running(false), is_paused(false)
    {
        init();
        editor_window_ptr = std::move(editor_window);
    }
    ~Engine()
    {
        glfwTerminate();
        this->cleanup();
    }
    bool isRunning() const noexcept
    {
        return this->is_running;
    }
    void run() const
    {
        while (this->editor_window_ptr->isOpen())
        {
            // Process Editor inputs
            this->process_input();

            // Rendering Scene


            // Window buffer Update
            this->editor_window_ptr->update();
        }
    }

    void set_is_paused(const bool isPaused)
    {
        this->is_paused = isPaused;
    }
    void set_fixed_timestep(const bool fixedTimeStep)
    {
        this->fixed_timestep = fixedTimeStep;
    }

};

#endif //ENGINE_HPP
