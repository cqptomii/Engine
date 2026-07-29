/**
 * @file editor_ui.hpp
 * @author FRAISSE Tom
 * @brief Editor UI class for the editor
 * @details This class is responsible for the UI of the editor
 * @version 0.1
 * @date 2026-07-29
 * @copyright Copyright (c) 2026
 */
#ifndef EDITOR_UI_HPP
#define EDITOR_UI_HPP

// GLFW includes
#include <GLFW/glfw3.h>

#include <functional>
#include <array>
#include <cstring>

// ImGUI includes
#include <imgui.h>
#include <imgui_internal.h>

// Engine includes
#include "engine/editor/ui/editor_context.hpp"
#include "engine/editor/picking/manipulation_mode.hpp"
#include "engine/editor/ui/viewport_framebuffer.hpp"
#include "engine/editor/ui/viewport_layout.hpp"
#include "engine/editor/ui/panels/hierarchy_panel.hpp"
#include "engine/editor/ui/panels/log_panel.hpp"
#include "engine/editor/ui/panels/properties_panel.hpp"

class EditorUI {
    // Titles for the differents panels
    static constexpr const char* VIEWPORT_TITLE = "Viewport";
    static constexpr const char* HIERARCHY_TITLE = "Scene Hierarchy";
    static constexpr const char* PROPERTIES_TITLE = "Node Properties";
    static constexpr const char* LOG_TITLE = "Log";

    // Flags for the differents panels
    bool show_viewport = true;
    bool show_hierarchy = true;
    bool show_properties = true;
    bool show_log = true;
    bool request_layout_reset = false;
    bool viewport_panel_open = false;
    bool viewport_image_hovered = false;

    // Flags for the scene file dialogs
    bool show_save_scene_dialog = false;
    bool show_load_scene_dialog = false;
    std::array<char, 512> scene_path_buffer{};
    bool scene_dialog_focus_pending = false;

    // Viewport framebuffer object
    ViewportFramebuffer viewport_framebuffer;
    // Viewport client bounds object
    ViewportClientBounds viewport_client_bounds{};

    // Flags for the viewport
    bool viewport_hovered = false;
    ImVec2 pending_viewport_size{0.0f, 0.0f};

    // Frame context pointer object
    const EditorUIContext* frame_context = nullptr;

    /**
     * @brief Method to build the default ImGui dock layout within the engine
     * 
     * @param dockspace_id The dockspace ID
     * @param dock_size The dock size
     */
    void build_default_dock_layout(const ImGuiID dockspace_id, const ImVec2 dock_size) {
        
        // Remove the existing dock node
        ImGui::DockBuilderRemoveNode(dockspace_id);
        // Add a new dock node
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        // Set the size of the dock node
        ImGui::DockBuilderSetNodeSize(dockspace_id, dock_size);

        // Intitializes the dock IDs
        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_right_id = 0;
        ImGuiID dock_bottom_id = 0;
        ImGuiID dock_right_bottom_id = 0;

        // Split the main dock node into a right / bottom / right bottom dock nodes
        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.22f, &dock_right_id, &dock_main_id);
        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.22f, &dock_bottom_id, &dock_main_id);
        ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.45f, &dock_right_bottom_id, &dock_right_id);

        // Dock the windows into the dock nodes
        ImGui::DockBuilderDockWindow(VIEWPORT_TITLE, dock_main_id);
        ImGui::DockBuilderDockWindow(LOG_TITLE, dock_bottom_id);
        ImGui::DockBuilderDockWindow(HIERARCHY_TITLE, dock_right_id);
        ImGui::DockBuilderDockWindow(PROPERTIES_TITLE, dock_right_bottom_id);
        
        // Finish the dock builder
        ImGui::DockBuilderFinish(dockspace_id);
    }

    /**
     * @brief Method to setup the default dock layout of the editor
     * 
     * @param dockspace_id The dockspace ID
     * @param dock_size The dock size
     */
    void setup_default_dock_layout(const ImGuiID dockspace_id, const ImVec2 dock_size) {
        // If the dockspace node is not valid, return
        if (ImGui::DockBuilderGetNode(dockspace_id) != nullptr) {
            return;
        }

        // Build the default dock layout
        this->build_default_dock_layout(dockspace_id, dock_size);
    }

    /**
     * @brief Method to reset the dock layout to the default editor layout
     * 
     * @param dockspace_id The dockspace ID
     * @param dock_size The dock size
     */
    void reset_dock_layout(const ImGuiID dockspace_id, const ImVec2 dock_size) {
        // Reset the flags for the differents panels
        this->show_viewport = true;
        this->show_hierarchy = true;
        this->show_properties = true;
        this->show_log = true;

        // Build the default dock layout
        this->build_default_dock_layout(dockspace_id, dock_size);
    }

    /**
     * @brief Method to draw the editor dockspace host window
     * 
     */
    void draw_dockspace_host() {
        // Get the Main viewport pointer
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        // Get the menu bar height
        const float menu_bar_height = ImGui::GetFrameHeight();

        // Get the dock position and size
        const ImVec2 dock_pos(viewport->Pos.x, viewport->Pos.y + menu_bar_height);
        const ImVec2 dock_size(viewport->Size.x, viewport->Size.y - menu_bar_height);

        // Set the next window position and size
        ImGui::SetNextWindowPos(dock_pos);
        ImGui::SetNextWindowSize(dock_size);
    
        // Update the Window viewport
        ImGui::SetNextWindowViewport(viewport->ID);

        // Set the host flags
        ImGuiWindowFlags host_flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoBackground;

        // Push the style variables for the window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        // Begin the editor dock host window
        ImGui::Begin("EditorDockHost", nullptr, host_flags);
        ImGui::PopStyleVar(3);

        // Get the dockspace ID
        const ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
        // Create the dockspace
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        
        // Setup the default dock layout
        this->setup_default_dock_layout(dockspace_id, dock_size);
        // If the request layout reset is true, reset the dock layout
        if (this->request_layout_reset) {
            // Reset the dock layout
            this->reset_dock_layout(dockspace_id, dock_size);
            // Reset the request layout reset flag
            this->request_layout_reset = false;
        }

        // End the editor dock host window
        ImGui::End();
    }

    /**
     * @brief Method to spawn a primitive from the UI
     * 
     * @param primitive_type The primitive type to spawn
     */
    void spawn_primitive_from_ui(const PrimitiveType primitive_type) {
        // If the frame context is not valid or the on spawn primitive callback is not valid, return
        if (this->frame_context != nullptr && this->frame_context->on_spawn_primitive) {
            // Call the on spawn primitive callback
            this->frame_context->on_spawn_primitive(primitive_type);
        }
    }

    /**
     * @brief Method to draw the top menu bar of the editor
     * 
     * @param window the pointer to the GLFW window
     */
    void draw_menu_bar(GLFWwindow* window) {
        // If the main menu bar is not valid, return
        if (!ImGui::BeginMainMenuBar()) {
            return;
        }

        // Create the File menu bar item
        if (ImGui::BeginMenu("File")) {
            
            // Create the New Scene menu bar item
            if (ImGui::MenuItem("New Scene", "Ctrl+N", false, false)) {
                if (this->frame_context != nullptr && this->frame_context->on_new_scene) {
                    this->frame_context->on_new_scene();
                }
            }

            // Create the Open Scene menu bar item
            if (ImGui::MenuItem("Open Scene...", "Ctrl+O", false, false)) {
                this->show_load_scene_dialog = true;
                this->scene_dialog_focus_pending = true;
            }

            // Create the Save Scene menu bar item
            if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, false)) {
                this->show_save_scene_dialog = true;
                this->scene_dialog_focus_pending = true;
            }
            ImGui::Separator();
            
            // Create the Exit menu bar item
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                if (window != nullptr) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
            }

            // End the "File" menu item
            ImGui::EndMenu();
        }

        // Create the Edit menu bar item
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo", "Ctrl+Z", false, false);
            ImGui::MenuItem("Redo", "Ctrl+Y", false, false);
            ImGui::Separator();
            ImGui::MenuItem("Preferences...", nullptr, false, false);
            ImGui::EndMenu();
        }

        // Create the Create menu bar item thus to spawn different primitives
        if (ImGui::BeginMenu("Create")) {

            // Cube
            if (ImGui::MenuItem("Cube")) {
                this->spawn_primitive_from_ui(PrimitiveType::Cube);
            }

            // Plane
            if (ImGui::MenuItem("Plane")) {
                this->spawn_primitive_from_ui(PrimitiveType::Plane);
            }

            // Sphere
            if (ImGui::MenuItem("Sphere")) {
                this->spawn_primitive_from_ui(PrimitiveType::Sphere);
            }

            // End the "Create" menu item
            ImGui::EndMenu();
        }

        // Create the Window menu bar item thus to show/hide different panels
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Viewport", nullptr, &this->show_viewport);
            ImGui::MenuItem("Scene Hierarchy", nullptr, &this->show_hierarchy);
            ImGui::MenuItem("Node Properties", nullptr, &this->show_properties);
            ImGui::MenuItem("Log", nullptr, &this->show_log);
            ImGui::Separator();

            // Create the Reset Layout menu bar item
            if (ImGui::MenuItem("Reset Layout")) {
                this->request_layout_reset = true;
            }
            ImGui::EndMenu();
        }

        // End the main menu bar
        ImGui::EndMainMenuBar();
    }

    /**
     * @brief Method to set the viewport client bounds from screen rect
     * 
     * @param rect_min The minimum rectangle position
     * @param rect_max The maximum rectangle position
     */
    void set_viewport_client_bounds_from_screen_rect(const ImVec2& rect_min, const ImVec2& rect_max) {
        // Get the main viewport pointer
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        // Set the viewport client bounds
        this->viewport_client_bounds.x = rect_min.x - main_viewport->Pos.x;
        this->viewport_client_bounds.y = rect_min.y - main_viewport->Pos.y;
        
        // Update the viewport client bounds width and height
        this->viewport_client_bounds.width = rect_max.x - rect_min.x;
        this->viewport_client_bounds.height = rect_max.y - rect_min.y;
    }

    /**
     * @brief Method to draw the log panel on the bottom DockNode of the editor
     * 
     */
    void draw_log_panel() {
        editor_ui::draw_log_panel(LOG_TITLE, this->show_log);
    }

    /**
     * @brief Method to draw the top Left ToolBar to change the manipulation mode in the editor
     * 
     */
    void draw_viewport_manipulation_toolbar() {
        // If the frame context is not valid or the on set manipulation mode callback is not valid, return
        if (this->frame_context == nullptr || !this->frame_context->on_set_manipulation_mode) {
            return;
        }

        const EditorUIContext& context = *this->frame_context;
        const ManipulationMode current_mode = context.get_manipulation_mode ? context.get_manipulation_mode(): ManipulationMode::NONE;

        // Create a lambda function to draw the manipulation mode button
        const auto draw_mode_button = [&](const char* label, const ManipulationMode mode, const bool active) {
            // If the mode is active, push the style colors for the button
            if (active) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.80f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.06f, 0.53f, 0.98f, 1.00f));
            }

            // If the button is clicked, call the on set manipulation mode callback
            if (ImGui::Button(label)) {
                context.on_set_manipulation_mode(mode);
            }

            if (active) {
                ImGui::PopStyleColor(3);
            }
        };

        // Check if the translate, rotate and scale modes are active
        const bool translate_active = current_mode == ManipulationMode::TRANSLATE;
        const bool rotate_active = current_mode == ManipulationMode::ROTATE;
        const bool scale_active = current_mode == ManipulationMode::SCALE_UNIFORM || current_mode == ManipulationMode::SCALE_NON_UNIFORM;

        // Draw the active manipulation mode buttons on the editor viewport
        draw_mode_button("Translate (G)", ManipulationMode::TRANSLATE, translate_active);
        ImGui::SameLine();
        draw_mode_button("Rotate (R)", ManipulationMode::ROTATE, rotate_active);
        ImGui::SameLine();
        draw_mode_button("Scale (S)", ManipulationMode::SCALE_UNIFORM, scale_active);
    }

    /**
     * @brief Method to draw the scene file loading and saving dialogs
     * 
     */
    void draw_scene_file_dialogs() {
        // If the scene path buffer is empty, set the default scene path
        if (this->scene_path_buffer[0] == '\0') {
            std::snprintf(
                this->scene_path_buffer.data(),
                this->scene_path_buffer.size(),
                "%s",
                "scenes/scene.scene"
            );
        }

        // If the save scene dialog is shown, open the save scene popup
        if (this->show_save_scene_dialog) {
            ImGui::OpenPopup("Save Scene");
            this->show_save_scene_dialog = false;
        }

        // If the load scene dialog is shown, open the load scene popup
        if (this->show_load_scene_dialog) {
            ImGui::OpenPopup("Load Scene");
            this->show_load_scene_dialog = false;
        }

        ImGui::SetNextWindowSize(ImVec2(420.0f, 0.0f), ImGuiCond_Appearing);

        if (ImGui::BeginPopupModal("Save Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted("Scene file path:");
            if (this->scene_dialog_focus_pending) {
                ImGui::SetKeyboardFocusHere();
                this->scene_dialog_focus_pending = false;
            }
            ImGui::InputText("##scene_save_path", this->scene_path_buffer.data(), this->scene_path_buffer.size());

            if (ImGui::Button("Save", ImVec2(120.0f, 0.0f))) {
                if (this->frame_context != nullptr && this->frame_context->on_save_scene) {
                    this->frame_context->on_save_scene(this->scene_path_buffer.data());
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Load Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted("Scene file path:");
            ImGui::InputText("##scene_load_path", this->scene_path_buffer.data(), this->scene_path_buffer.size());

            if (ImGui::Button("Load", ImVec2(120.0f, 0.0f))) {
                if (this->frame_context != nullptr && this->frame_context->on_load_scene) {
                    this->frame_context->on_load_scene(this->scene_path_buffer.data());
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

public:
    /**
     * @brief Method to begin the frame UI
     * 
     * @param window The GLFW window pointer
     * @param context The editor UI context
     */
    void begin_frame_ui(GLFWwindow* window, const EditorUIContext& context) {
        // Get the fram context pointer
        this->frame_context = &context;

        // Draw each of the editor UI elements
        this->draw_menu_bar(window);
        this->draw_scene_file_dialogs();
        this->draw_dockspace_host();

        // Draw the hierarchy panel on the right DockNode of the editor
        editor_ui::draw_hierarchy_panel(HIERARCHY_TITLE, this->show_hierarchy, context);
        editor_ui::draw_properties_panel(PROPERTIES_TITLE, this->show_properties, context);

        // Draw the log panel on the bottom DockNode of the editor
        this->draw_log_panel();
    }


    /**
     * @brief Method to begin the viewport panel
     * 
     * @param layout The viewport layout
     * @return true if the viewport panel is visible
     * @return false if the viewport panel is not visible
     */
    bool begin_viewport_panel(ViewportLayout& layout) {
        // Set the viewport layout properties
        layout.visible = this->show_viewport;
        layout.hovered = false;
        layout.render_width = 0;
        layout.render_height = 0;
        this->viewport_hovered = false;
        this->viewport_panel_open = false;

        // If the viewport is not visible, return false
        if (!this->show_viewport) {
            return false;
        }

        // Create the viewport flags
        const ImGuiWindowFlags viewport_flags =
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;

        // Begin the viewport panel
        if (!ImGui::Begin(VIEWPORT_TITLE, nullptr, viewport_flags)) {
            ImGui::End();
            return false;
        }

        this->viewport_panel_open = true;
        // Draw the viewport manipulation toolbar
        this->draw_viewport_manipulation_toolbar();

        // Get the available region size for the viewport
        this->pending_viewport_size = ImGui::GetContentRegionAvail();
        // Update the viewport layout render width
        layout.render_width = static_cast<int>(this->pending_viewport_size.x);
        layout.render_height = static_cast<int>(this->pending_viewport_size.y);
        layout.hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
        this->viewport_hovered = layout.hovered;

        // Return true if the viewport panel is visible
        return true;
    }

    /**
     * @brief Method to render the viewport panel
     * 
     * @param render_viewport The viewport render callback
     */
    void end_viewport_panel(const std::function<void(const ViewportLayout&)>& render_viewport) {
        if (!this->viewport_panel_open) {
            return;
        }

        ViewportLayout layout{};

        // Set the viewport layout properties
        layout.visible = true;
        layout.render_width = static_cast<int>(this->pending_viewport_size.x);
        layout.render_height = static_cast<int>(this->pending_viewport_size.y);
        layout.hovered = this->viewport_hovered;

        // Check if the viewport render size is valid and the render viewport callback is valid
        if (layout.render_width > 0 && layout.render_height > 0 && render_viewport) {
            this->viewport_framebuffer.resize(layout.render_width, layout.render_height);
            render_viewport(layout);

            // If the viewport framebuffer is valid, draw the viewport image
            if (this->viewport_framebuffer.is_valid()) {
                const ImTextureID texture_id = static_cast<ImTextureID>(
                    static_cast<intptr_t>(this->viewport_framebuffer.get_color_texture_id())
                );

                // Draw the viewport image
                ImGui::Image(
                    texture_id,
                    this->pending_viewport_size,
                    ImVec2(0.0f, 1.0f),
                    ImVec2(1.0f, 0.0f)
                );

                // Spawn a primitive from the UI context menu if the viewport is right-clicked
                if (ImGui::BeginPopupContextItem("ViewportContextMenu")) {
                    // Create the Add menu item
                    if (ImGui::BeginMenu("Add")) {
                        // Create the Mesh menu item
                        if (ImGui::BeginMenu("Mesh")) {
                            // Create the Cube menu item
                            if (ImGui::MenuItem("Cube")) {
                                this->spawn_primitive_from_ui(PrimitiveType::Cube);
                            }
                            // Create the Plane menu item
                            if (ImGui::MenuItem("Plane")) {
                                this->spawn_primitive_from_ui(PrimitiveType::Plane);
                            }
                            // Create the Sphere menu item
                            if (ImGui::MenuItem("Sphere")) {
                                this->spawn_primitive_from_ui(PrimitiveType::Sphere);
                            }
                            ImGui::EndMenu();
                        }
                        // End the "Add" menu item
                        ImGui::EndMenu();
                    }
                    ImGui::EndPopup();
                }

                // Set the viewport client bounds from the screen rect
                this->set_viewport_client_bounds_from_screen_rect(
                    ImGui::GetItemRectMin(),
                    ImGui::GetItemRectMax()
                );

                // Update the viewport image hovered flag
                this->viewport_image_hovered = ImGui::IsItemHovered();

                // Check if the frame context is valid
                if (this->frame_context != nullptr) {
                    const ImVec2 overlay_min = ImGui::GetItemRectMin();
                    const ImVec2 overlay_max = ImGui::GetItemRectMax();

                    // Get the current manipulation mode
                    const ManipulationMode current_mode = this->frame_context->get_manipulation_mode? this->frame_context->get_manipulation_mode() : ManipulationMode::NONE;
                    const char* mode_label = manipulation_mode_label(current_mode);
 
                    // Get the draw list for the viewport
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    const ImVec2 text_pos(overlay_min.x + 8.0f, overlay_max.y - 28.0f);
                    const ImVec2 text_size = ImGui::CalcTextSize(mode_label);
                    const ImVec2 bg_min(text_pos.x - 6.0f, text_pos.y - 4.0f);
                    const ImVec2 bg_max(text_pos.x + text_size.x + 6.0f, text_pos.y + text_size.y + 4.0f);

                    // Draw the background rectangle for the manipulation mode label
                    draw_list->AddRectFilled(bg_min, bg_max, IM_COL32(20, 20, 20, 180), 4.0f);
                    
                    // Draw the manipulation mode label
                    draw_list->AddText(text_pos, IM_COL32(240, 240, 240, 255), mode_label);
                }
            }
        } else {
            ImGui::TextDisabled("Viewport size is too small.");
            this->viewport_image_hovered = false;
        }

        ImGui::End();
        this->viewport_panel_open = false;
    }

    /**
     * @brief Method to get the viewport framebuffer object
     * 
     * @return ViewportFramebuffer& The viewport framebuffer object
     */
    [[nodiscard]] ViewportFramebuffer& get_viewport_framebuffer() noexcept {
        return this->viewport_framebuffer;
    }

    /**
     * @brief Method to get the viewport client bounds object
     * 
     * @return const ViewportClientBounds& The viewport client bounds object
     */
    [[nodiscard]] const ViewportClientBounds& get_viewport_client_bounds() const noexcept {
        return this->viewport_client_bounds;
    }

    /**
     * @brief Method to check if the viewport allows mouse input
     * 
     * @return true if the viewport allows mouse input
     * @return false if the viewport does not allow mouse input
     */
    [[nodiscard]] bool allows_viewport_mouse_input() const noexcept {
        return this->viewport_image_hovered
            && this->viewport_client_bounds.is_valid();
    }

    /**
     * @brief Method to check if the viewport wants to capture the keyboard inputs
     * 
     * @return true if the viewport wants to capture the keyboard
     * @return false if the viewport does not want to capture the keyboard
     */
    [[nodiscard]] bool allows_viewport_keyboard_input() const noexcept {
        const ImGuiIO& io = ImGui::GetIO();
        if (io.WantTextInput) {
            return false;
        }
        if (!io.WantCaptureKeyboard) {
            return true;
        }
        return this->viewport_image_hovered && this->viewport_client_bounds.is_valid();
    }

    /**
     * @brief Method to check if the viewport wants to capture the mouse inputs
     * 
     * @return true if the viewport wants to capture the mouse
     * @return false if the viewport does not want to capture the mouse
     */
    [[nodiscard]] bool wants_capture_mouse() const {
        return ImGui::GetIO().WantCaptureMouse;
    }

    /**
     * @brief Method to check if the viewport wants to capture the keyboard inputs
     * 
     * @return true if the viewport wants to capture the mouse
     * @return false if the viewport does not want to capture the mouse
     */
    [[nodiscard]] bool wants_capture_keyboard() const {
        return ImGui::GetIO().WantCaptureKeyboard;
    }
};

#endif
