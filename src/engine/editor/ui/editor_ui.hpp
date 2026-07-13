#ifndef EDITOR_UI_HPP
#define EDITOR_UI_HPP

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_internal.h>

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
        ImGui::DockSpace(
            dockspace_id,
            ImVec2(0.0f, 0.0f),
            ImGuiDockNodeFlags_PassthruCentralNode
        );
        this->setup_default_dock_layout(dockspace_id, dock_size);
        if (this->request_layout_reset) {
            this->reset_dock_layout(dockspace_id, dock_size);
            this->request_layout_reset = false;
        }

        ImGui::End();
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
            ImGui::MenuItem("Empty Object", nullptr, false, false);
            ImGui::MenuItem("Cube", nullptr, false, false);
            ImGui::MenuItem("Sphere", nullptr, false, false);
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

    void draw_viewport_panel() {
        if (!this->show_viewport) {
            return;
        }

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        const ImGuiWindowFlags viewport_flags =
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoScrollbar;

        if (ImGui::Begin(VIEWPORT_TITLE, nullptr, viewport_flags)) {
            const ImVec2 content_size = ImGui::GetContentRegionAvail();
            ImGui::TextDisabled("Viewport overlay (scene 3D visible behind).");
            ImGui::Text("Content area: %.0f x %.0f", content_size.x, content_size.y);
        }
        ImGui::End();
        ImGui::PopStyleColor();
    }

    void draw_hierarchy_panel() {
        if (!this->show_hierarchy) {
            return;
        }

        if (ImGui::Begin(HIERARCHY_TITLE, &this->show_hierarchy)) {
            ImGui::TextUnformatted("Cube");
            ImGui::TextUnformatted("Sphere");
        }
        ImGui::End();
    }

    void draw_properties_panel() {
        if (!this->show_properties) {
            return;
        }

        if (ImGui::Begin(PROPERTIES_TITLE, &this->show_properties)) {
            ImGui::TextDisabled("No object selected.");
            ImGui::Separator();
            ImGui::Text("Transform");
            ImGui::Text("Position");
            ImGui::Text("Rotation");
            ImGui::Text("Scale");
        }
        ImGui::End();
    }

    void draw_log_panel() {
        if (!this->show_log) {
            return;
        }

        if (ImGui::Begin(LOG_TITLE, &this->show_log)) {
            ImGui::TextUnformatted("[Info][Engine] Editor UI initialized.");
        }
        ImGui::End();
    }

public:
    void render(GLFWwindow* window, const int /*framebuffer_width*/, const int /*framebuffer_height*/) {
        this->draw_menu_bar(window);
        this->draw_dockspace_host();
        this->draw_viewport_panel();
        this->draw_hierarchy_panel();
        this->draw_properties_panel();
        this->draw_log_panel();
    }

    [[nodiscard]] bool wants_capture_mouse() const {
        return ImGui::GetIO().WantCaptureMouse;
    }

    [[nodiscard]] bool wants_capture_keyboard() const {
        return ImGui::GetIO().WantCaptureKeyboard;
    }
};

#endif
