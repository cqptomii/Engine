#ifndef ENGINE_LOG_PANEL_HPP
#define ENGINE_LOG_PANEL_HPP

#include <imgui.h>
#include <string>

#include "engine/core/debug/instrumentation.hpp"
#include "engine/editor/ui/panels/debug_panel.hpp"

namespace editor_ui {

inline void draw_console_tab() {
    if (ImGui::Button("Clear")) {
        Instrumentation::logger().clear_buffer();
    }

    ImGui::SameLine();
    ImGui::TextDisabled("Scrollable application logs");

    const ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
    if (ImGui::BeginChild("ConsoleScroll", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, child_flags)) {
        const LogBuffer& buffer = Instrumentation::logger().get_buffer();
        for (const LogEntry& entry : buffer.get_entries()) {
            ImGui::PushStyleColor(ImGuiCol_Text, log_level_color(entry.level));
            ImGui::TextUnformatted(
                ("[" + std::string(log_level_label(entry.level)) + "][" + entry.category + "] " + entry.message).c_str()
            );
            ImGui::PopStyleColor();
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();
}

inline void draw_log_panel(const char* title, bool& show_log) {
    if (!show_log) {
        return;
    }

    if (ImGui::Begin(title, &show_log)) {
        if (ImGui::BeginTabBar("LogTabs")) {
            if (ImGui::BeginTabItem("Console")) {
                draw_console_tab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Debug")) {
                draw_debug_live_panel();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

} // namespace editor_ui

#endif
