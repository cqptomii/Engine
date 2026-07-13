#ifndef ENGINE_DEBUG_PANEL_HPP
#define ENGINE_DEBUG_PANEL_HPP

#include <algorithm>
#include <imgui.h>
#include <utility>
#include <vector>

#include "engine/core/debug/debug_config.hpp"
#include "engine/core/debug/debug_id.hpp"
#include "engine/core/debug/instrumentation.hpp"
#include "engine/core/debug/log/log_types.hpp"

namespace editor_ui {

inline ImVec4 log_level_color(const LogLevel level) {
    switch (level) {
        case LogLevel::Trace:
            return ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
        case LogLevel::Info:
            return ImVec4(0.90f, 0.90f, 0.90f, 1.0f);
        case LogLevel::Warning:
            return ImVec4(1.0f, 0.82f, 0.35f, 1.0f);
        case LogLevel::Error:
            return ImVec4(1.0f, 0.45f, 0.45f, 1.0f);
        default:
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

inline const char* log_level_label(const LogLevel level) {
    switch (level) {
        case LogLevel::Trace:
            return "Trace";
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warning:
            return "Warning";
        case LogLevel::Error:
            return "Error";
        default:
            return "Unknown";
    }
}

inline void draw_debug_live_panel() {
#ifdef ENGINE_DEBUG
    const FrameStats& frame_stats = Instrumentation::frame_stats();
    const float frame_ms = frame_stats.get_frame_ms();
    const float delta_time = frame_stats.get_delta_time();
    const float fps = delta_time > 0.0f ? 1.0f / delta_time : 0.0f;

    ImGui::SeparatorText("Frame");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame time: %.3f ms", frame_ms);
    ImGui::Text("Delta time: %.4f s", delta_time);
    ImGui::Text("Smoothed FPS: %.1f", Instrumentation::console().get_fps());

    ImGui::SeparatorText("Profiler (last frame)");
    const Profiler& profiler = Instrumentation::profiler();
    if (profiler.scopes().empty()) {
        ImGui::TextDisabled("No scoped samples.");
    } else {
        for (const auto& [scope_name, elapsed_ms] : profiler.scopes()) {
            ImGui::Text("%s: %.3f ms", scope_name.c_str(), elapsed_ms);
        }
        ImGui::Text("Scoped total: %.3f ms", profiler.frame_total());
    }

    ImGui::SeparatorText("Counters (current frame)");
    const CounterRegistry& counters = Instrumentation::counters();
    bool has_counters = false;
    for (const auto& [key, value] : counters.get_system_gauges()) {
        has_counters = true;
        ImGui::Text("%s: %u", key.c_str(), value);
    }
    for (const auto& [key, value] : counters.get_system_counters()) {
        has_counters = true;
        ImGui::Text("%s: %u", key.c_str(), value);
    }
    if (!has_counters) {
        ImGui::TextDisabled("No counters this frame.");
    }

    ImGui::SeparatorText("Events (current frame)");
    ImGui::Text("Total: %u", Instrumentation::event_stats().get_frame_total_events());

    ImGui::SeparatorText("Memory");
    const MemoryTracker& memory = Instrumentation::memory();
    const CpuResourceCounts& cpu = memory.cpu();
    const GpuResourceCounts& gpu = memory.gpu();
    ImGui::Text("CPU models: %zu", cpu.models);
    ImGui::Text("CPU meshes: %zu", cpu.meshes);
    ImGui::Text("CPU materials: %zu", cpu.materials);
    ImGui::Text("CPU material instances: %zu", cpu.material_instances);
    ImGui::Text("CPU textures: %zu", cpu.textures);
    ImGui::Text("CPU shaders: %zu", cpu.shaders);
    ImGui::Text("GPU meshes: %zu", gpu.meshes);
    ImGui::Text("GPU textures: %zu", gpu.textures);
    ImGui::Text("GPU shaders: %zu", gpu.shaders);
    ImGui::Text("GPU UBOs: %zu", gpu.ubos);

    ImGui::SeparatorText("Debug names");
    const DebugNameRegistry& names = Instrumentation::names();
    ImGui::Text("Registered: %zu", names.count());

    std::vector<std::pair<DebugId, std::string>> debug_names = names.get_entries();
    std::sort(
        debug_names.begin(),
        debug_names.end(),
        [](const std::pair<DebugId, std::string>& lhs, const std::pair<DebugId, std::string>& rhs) {
            return lhs.second < rhs.second;
        }
    );

    if (debug_names.empty()) {
        ImGui::TextDisabled("No debug names registered.");
    } else if (ImGui::BeginTable("DebugNamesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0.0f, 120.0f))) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Id");
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (const auto& [id, name] : debug_names) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%u", id.value());
        }

        ImGui::EndTable();
    }
#else
    ImGui::TextDisabled("Debug panel requires a Debug build (ENGINE_DEBUG).");
#endif
}

} // namespace editor_ui

#endif
