/**
 * @file imgui_layer.hpp
 * @author Tom FRAISSE
 * @brief Header file for the ImGui layer
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef IMGUI_LAYER_HPP
#define IMGUI_LAYER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

/**
 * @brief Class representing the ImGui layer
 */
class ImGuiLayer {
    bool initialized = false;

public:
    ImGuiLayer() = default;

    /**
     * @brief Initialize the ImGui layer
     * @param window The window to initialize the ImGui layer for
     * @return void
     */
    void init(GLFWwindow* window) {
        if (this->initialized || window == nullptr) {
            return;
        }

        // Check the ImGui version and create the ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Get the ImGui IO
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460 core");

        this->initialized = true;
    }

    /**
     * @brief Begin the ImGui frame
     * @return void
     */
    void begin_frame() {
        // Begin the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    /**
     * @brief End the ImGui frame
     * @return void
     */
    void end_frame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /**
     * @brief Shutdown the ImGui layer
     * 
     */
    void shutdown() {
        // Check if the ImGui layer is not initialized
        if (!this->initialized) {
            return;
        }

        // Shutdown the ImGui layer
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        this->initialized = false;
    }

    /**
     * @brief Check if the ImGui layer is initialized
     * 
     * @return true If the ImGui layer is initialized
     * @return false If the ImGui layer is not initialized
     */
    [[nodiscard]] bool is_initialized() const noexcept {
        // Return the initialized state
        return this->initialized;
    }
};

#endif
