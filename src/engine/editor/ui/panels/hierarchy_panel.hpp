#ifndef ENGINE_HIERARCHY_PANEL_HPP
#define ENGINE_HIERARCHY_PANEL_HPP

#include <algorithm>
#include <imgui.h>
#include <string>
#include <vector>

#include <entt/entt.hpp>

#include "engine/ecs/components/debug_name_component.hpp"
#include "engine/editor/ui/editor_context.hpp"

namespace editor_ui {

inline void draw_hierarchy_panel(const char* title, bool& show_panel, const EditorUIContext& context) {
    if (!show_panel) {
        return;
    }

    if (!ImGui::Begin(title, &show_panel)) {
        ImGui::End();
        return;
    }

    struct HierarchyEntry {
        entt::entity entity = entt::null;
        std::string name;
    };

    std::vector<HierarchyEntry> entries;
    auto& registry = context.scene.get_registry().raw();
    for (const auto entity : registry.view<DebugNameComponent>()) {
        entries.push_back({
            entity,
            registry.get<DebugNameComponent>(entity).get_name()
        });
    }

    std::sort(entries.begin(), entries.end(), [](const HierarchyEntry& lhs, const HierarchyEntry& rhs) {
        return lhs.name < rhs.name;
    });

    if (entries.empty()) {
        ImGui::TextDisabled("No scene objects.");
    } else {
        for (const HierarchyEntry& entry : entries) {
            const bool is_selected = std::find(
                context.selected_entities.begin(),
                context.selected_entities.end(),
                entry.entity
            ) != context.selected_entities.end();

            if (ImGui::Selectable(entry.name.c_str(), is_selected)) {
                if (context.on_select_entity) {
                    context.on_select_entity(entry.entity);
                }
            }
        }
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
        if (context.on_clear_selection) {
            context.on_clear_selection();
        }
    }

    ImGui::End();
}

} // namespace editor_ui

#endif
