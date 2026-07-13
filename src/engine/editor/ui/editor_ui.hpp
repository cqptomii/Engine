#ifndef EDITOR_UI_HPP
#define EDITOR_UI_HPP

#include <GLFW/glfw3.h>

#include <functional>

#include <imgui.h>
#include <imgui_internal.h>

#include "engine/editor/ui/editor_context.hpp"
#include "engine/editor/ui/viewport_framebuffer.hpp"
#include "engine/editor/ui/viewport_layout.hpp"
#include "engine/editor/ui/panels/hierarchy_panel.hpp"
#include "engine/editor/ui/panels/log_panel.hpp"
#include "engine/editor/ui/panels/properties_panel.hpp"

class EditorUI {
    static constexpr const char* VIEWPORT_TITLE = "Viewport";
    static constexpr const char* HIERARCHY_TITLE = "Scene Hierarchy";
    static constexpr const char* PROPERTIES_TITLE = "Node Properties";
    static constexpr const char* LOG_TITLE = "Log";

    bool show_viewport = true;
    bool show_hierarchy = true;
    bool show_properties = true;
    bool show_log = true;
    bool request_layout_reset = false;
    bool viewport_panel_open = false;
    bool viewport_image_hovered = false;

    ViewportFramebuffer viewport_framebuffer;
    ViewportClientBounds viewport_client_bounds{};
    bool viewport_hovered = false;
    ImVec2 pending_viewport_size{0.0f, 0.0f};
    const EditorUIContext* frame_context = nullptr;

    void build_default_dock_layout(const ImGuiID dockspace_id, const ImVec2 dock_size) {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, dock_size);

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_right_id = 0;
        ImGuiID dock_bottom_id = 0;
        ImGuiID dock_right_bottom_id = 0;

        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.22f, &dock_right_id, &dock_main_id);
        ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.22f, &dock_bottom_id, &dock_main_id);
        ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.45f, &dock_right_bottom_id, &dock_right_id);

        ImGui::DockBuilderDockWindow(VIEWPORT_TITLE, dock_main_id);
        ImGui::DockBuilderDockWindow(LOG_TITLE, dock_bottom_id);
        ImGui::DockBuilderDockWindow(HIERARCHY_TITLE, dock_right_id);
        ImGui::DockBuilderDockWindow(PROPERTIES_TITLE, dock_right_bottom_id);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    void setup_default_dock_layout(const ImGuiID dockspace_id, const ImVec2 dock_size) {
        if (ImGui::DockBuilderGetNode(dockspace_id) != nullptr) {
            return;
        }

        this->build_default_dock_layout(dockspace_id, dock_size);
    }

    void reset_dock_layout(const ImGuiID dockspace_id, const ImVec2 dock_size) {
        this->show_viewport = true;
        this->show_hierarchy = true;
        this->show_properties = true;
        this->show_log = true;
        this->build_default_dock_layout(dockspace_id, dock_size);
    }

    void draw_dockspace_host() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const float menu_bar_height = ImGui::GetFrameHeight();
        const ImVec2 dock_pos(viewport->Pos.x, viewport->Pos.y + menu_bar_height);
        const ImVec2 dock_size(viewport->Size.x, viewport->Size.y - menu_bar_height);

        ImGui::SetNextWindowPos(dock_pos);
        ImGui::SetNextWindowSize(dock_size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags host_flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("EditorDockHost", nullptr, host_flags);
        ImGui::PopStyleVar(3);

        const ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        this->setup_default_dock_layout(dockspace_id, dock_size);
        if (this->request_layout_reset) {
            this->reset_dock_layout(dockspace_id, dock_size);
            this->request_layout_reset = false;
        }

        ImGui::End();
    }

    void spawn_primitive_from_ui(const PrimitiveType primitive_type) {
        if (this->frame_context != nullptr && this->frame_context->on_spawn_primitive) {
            this->frame_context->on_spawn_primitive(primitive_type);
        }
    }

    void draw_menu_bar(GLFWwindow* window) {
        if (!ImGui::BeginMainMenuBar()) {
            return;
        }

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N", false, false)) {
            }
            if (ImGui::MenuItem("Open Scene...", "Ctrl+O", false, false)) {
            }
            if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, false)) {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                if (window != nullptr) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo", "Ctrl+Z", false, false);
            ImGui::MenuItem("Redo", "Ctrl+Y", false, false);
            ImGui::Separator();
            ImGui::MenuItem("Preferences...", nullptr, false, false);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Create")) {
            if (ImGui::MenuItem("Cube")) {
                this->spawn_primitive_from_ui(PrimitiveType::Cube);
            }
            if (ImGui::MenuItem("Plane")) {
                this->spawn_primitive_from_ui(PrimitiveType::Plane);
            }
            if (ImGui::MenuItem("Sphere")) {
                this->spawn_primitive_from_ui(PrimitiveType::Sphere);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Viewport", nullptr, &this->show_viewport);
            ImGui::MenuItem("Scene Hierarchy", nullptr, &this->show_hierarchy);
            ImGui::MenuItem("Node Properties", nullptr, &this->show_properties);
            ImGui::MenuItem("Log", nullptr, &this->show_log);
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Layout")) {
                this->request_layout_reset = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    void set_viewport_client_bounds_from_screen_rect(const ImVec2& rect_min, const ImVec2& rect_max) {
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        this->viewport_client_bounds.x = rect_min.x - main_viewport->Pos.x;
        this->viewport_client_bounds.y = rect_min.y - main_viewport->Pos.y;
        this->viewport_client_bounds.width = rect_max.x - rect_min.x;
        this->viewport_client_bounds.height = rect_max.y - rect_min.y;
    }

    void draw_log_panel() {
        editor_ui::draw_log_panel(LOG_TITLE, this->show_log);
    }

public:
    void begin_frame_ui(GLFWwindow* window, const EditorUIContext& context) {
        this->frame_context = &context;
        this->draw_menu_bar(window);
        this->draw_dockspace_host();
        editor_ui::draw_hierarchy_panel(HIERARCHY_TITLE, this->show_hierarchy, context);
        editor_ui::draw_properties_panel(PROPERTIES_TITLE, this->show_properties, context);
        this->draw_log_panel();
    }

    bool begin_viewport_panel(ViewportLayout& layout) {
        layout.visible = this->show_viewport;
        layout.hovered = false;
        layout.render_width = 0;
        layout.render_height = 0;
        this->viewport_hovered = false;
        this->viewport_panel_open = false;

        if (!this->show_viewport) {
            return false;
        }

        const ImGuiWindowFlags viewport_flags =
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;

        if (!ImGui::Begin(VIEWPORT_TITLE, nullptr, viewport_flags)) {
            ImGui::End();
            return false;
        }

        this->viewport_panel_open = true;
        this->pending_viewport_size = ImGui::GetContentRegionAvail();
        layout.render_width = static_cast<int>(this->pending_viewport_size.x);
        layout.render_height = static_cast<int>(this->pending_viewport_size.y);
        layout.hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
        this->viewport_hovered = layout.hovered;

        return true;
    }

    void end_viewport_panel(const std::function<void(const ViewportLayout&)>& render_viewport) {
        if (!this->viewport_panel_open) {
            return;
        }

        ViewportLayout layout{};
        layout.visible = true;
        layout.render_width = static_cast<int>(this->pending_viewport_size.x);
        layout.render_height = static_cast<int>(this->pending_viewport_size.y);
        layout.hovered = this->viewport_hovered;

        if (layout.render_width > 0 && layout.render_height > 0 && render_viewport) {
            this->viewport_framebuffer.resize(layout.render_width, layout.render_height);
            render_viewport(layout);

            if (this->viewport_framebuffer.is_valid()) {
                const ImTextureID texture_id = static_cast<ImTextureID>(
                    static_cast<intptr_t>(this->viewport_framebuffer.get_color_texture_id())
                );
                ImGui::Image(
                    texture_id,
                    this->pending_viewport_size,
                    ImVec2(0.0f, 1.0f),
                    ImVec2(1.0f, 0.0f)
                );
                if (ImGui::BeginPopupContextItem("ViewportContextMenu")) {
                    if (ImGui::BeginMenu("Add")) {
                        if (ImGui::BeginMenu("Mesh")) {
                            if (ImGui::MenuItem("Cube")) {
                                this->spawn_primitive_from_ui(PrimitiveType::Cube);
                            }
                            if (ImGui::MenuItem("Plane")) {
                                this->spawn_primitive_from_ui(PrimitiveType::Plane);
                            }
                            if (ImGui::MenuItem("Sphere")) {
                                this->spawn_primitive_from_ui(PrimitiveType::Sphere);
                            }
                            ImGui::EndMenu();
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndPopup();
                }
                this->set_viewport_client_bounds_from_screen_rect(
                    ImGui::GetItemRectMin(),
                    ImGui::GetItemRectMax()
                );
                this->viewport_image_hovered = ImGui::IsItemHovered();
            }
        } else {
            ImGui::TextDisabled("Viewport size is too small.");
            this->viewport_image_hovered = false;
        }

        ImGui::End();
        this->viewport_panel_open = false;
    }

    [[nodiscard]] ViewportFramebuffer& get_viewport_framebuffer() noexcept {
        return this->viewport_framebuffer;
    }

    [[nodiscard]] const ViewportClientBounds& get_viewport_client_bounds() const noexcept {
        return this->viewport_client_bounds;
    }

    [[nodiscard]] bool allows_viewport_mouse_input() const noexcept {
        return this->viewport_image_hovered
            && this->viewport_client_bounds.is_valid();
    }

    [[nodiscard]] bool allows_viewport_keyboard_input() const noexcept {
        return !ImGui::GetIO().WantCaptureKeyboard;
    }

    [[nodiscard]] bool wants_capture_mouse() const {
        return ImGui::GetIO().WantCaptureMouse;
    }

    [[nodiscard]] bool wants_capture_keyboard() const {
        return ImGui::GetIO().WantCaptureKeyboard;
    }
};

#endif
