/**
 * @file log_panel.hpp
 * @author Tom FRAISSE
 * @brief Header file for the log panel
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_LOG_PANEL_HPP
#define ENGINE_LOG_PANEL_HPP

#include <imgui.h>
#include <string>

#include "engine/core/debug/instrumentation.hpp"
#include "engine/editor/ui/panels/debug_panel.hpp"

namespace editor_ui {

/**
 * @brief Method to draw the console tab and its contents onto the ImGui window
 * 
 */
inline void draw_console_tab() {
    // Check if the clear button is clicked
    if (ImGui::Button("Clear")) {
        Instrumentation::logger().clear_buffer();
    }

    // Draw the scrollable application logs text
    ImGui::SameLine();
    ImGui::TextDisabled("Scrollable application logs");

    // Draw the scrollable application logs text
    const ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
    if (ImGui::BeginChild("ConsoleScroll", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, child_flags)) {
        const LogBuffer& buffer = Instrumentation::logger().get_buffer();
        // Draw the log entries
        for (const LogEntry& entry : buffer.get_entries()) {
            // Draw the log entry
            ImGui::PushStyleColor(ImGuiCol_Text, log_level_color(entry.level));
            ImGui::TextUnformatted(
                ("[" + std::string(log_level_label(entry.level)) + "][" + entry.category + "] " + entry.message).c_str()
            );
            ImGui::PopStyleColor();
        }

        // Check if the scroll is at the bottom
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    // End the scrollable application logs text
    ImGui::EndChild();
}

/**
 * @brief Method to draw the log panel and its contents onto the ImGui window
 * 
 * @param title The title of the log panel
 * @param show_log A reference to the boolean that controls whether the log panel is shown
 * @return void
 * @param show_log 
 */
inline void draw_log_panel(const char* title, bool& show_log) {
    // Check if the log panel is not shown
    if (!show_log) {
        return;
    }

    // Check if the log panel is opened
    if (ImGui::Begin(title, &show_log)) {
        if (ImGui::BeginTabBar("LogTabs")) {
            // Check if the console tab is opened
            if (ImGui::BeginTabItem("Console")) {
                // Draw the console tab
                draw_console_tab();
                ImGui::EndTabItem();
            }

            // Check if the debug tab is opened
            if (ImGui::BeginTabItem("Debug")) {
                // Draw the debug live panel
                draw_debug_live_panel();
                ImGui::EndTabItem();
            }

            // End the tab bar
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

} // namespace editor_ui

#endif
