#ifndef ENGINE_HIERARCHY_PANEL_HPP
#define ENGINE_HIERARCHY_PANEL_HPP

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <imgui.h>
#include <string>

#include <entt/entt.hpp>

#include "engine/ecs/components/debug_name_component.hpp"
#include "engine/ecs/components/scene_root_component.hpp"
#include "engine/editor/ui/editor_context.hpp"
#include "engine/scene/scene_hierarchy.hpp"

namespace editor_ui {

namespace {

constexpr const char* k_hierarchy_drag_payload = "HIERARCHY_ENTITY";

struct HierarchyRenameState {
    entt::entity entity = entt::null;
    std::array<char, 128> buffer{};
    bool focus_next_frame = false;
};

inline HierarchyRenameState& hierarchy_rename_state() {
    static HierarchyRenameState state{};
    return state;
}

[[nodiscard]] inline std::string entity_display_name(const Scene& scene, const entt::entity entity) {
    const entt::registry& registry = scene.get_registry().raw();
    if (!registry.valid(entity)) {
        return "Invalid";
    }
    if (registry.all_of<DebugNameComponent>(entity)) {
        return registry.get<DebugNameComponent>(entity).get_name();
    }
    return "Entity " + std::to_string(entt::to_integral(entity));
}

inline std::string trim_string(const std::string& value) {
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

inline void begin_hierarchy_rename(const EditorUIContext& context, const entt::entity entity) {
    if (!context.on_rename_entity) {
        return;
    }

    HierarchyRenameState& rename_state = hierarchy_rename_state();
    rename_state.entity = entity;
    rename_state.focus_next_frame = true;

    const std::string current_name = entity_display_name(context.scene, entity);
    std::snprintf(rename_state.buffer.data(), rename_state.buffer.size(), "%s", current_name.c_str());
}

inline void commit_hierarchy_rename(const EditorUIContext& context) {
    HierarchyRenameState& rename_state = hierarchy_rename_state();
    if (rename_state.entity == entt::null || !context.on_rename_entity) {
        rename_state.entity = entt::null;
        return;
    }

    const std::string new_name = trim_string(rename_state.buffer.data());
    if (!new_name.empty()) {
        context.on_rename_entity(rename_state.entity, new_name);
    }

    rename_state.entity = entt::null;
    rename_state.focus_next_frame = false;
}

inline void cancel_hierarchy_rename() {
    HierarchyRenameState& rename_state = hierarchy_rename_state();
    rename_state.entity = entt::null;
    rename_state.focus_next_frame = false;
}

inline void draw_hierarchy_drag_drop_targets(const EditorUIContext& context, const entt::entity target_parent) {
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(k_hierarchy_drag_payload)) {
            const auto dragged_entity = *static_cast<const entt::entity*>(payload->Data);
            if (context.on_reparent_entity
                && dragged_entity != entt::null
                && dragged_entity != target_parent
                && !scene_hierarchy::would_create_cycle(context.scene, dragged_entity, target_parent)) {
                context.on_reparent_entity(dragged_entity, target_parent);
            }
        }
        ImGui::EndDragDropTarget();
    }
}

inline void draw_hierarchy_node(const EditorUIContext& context, const entt::entity entity) {
    const entt::registry& registry = context.scene.get_registry().raw();
    if (!registry.valid(entity)) {
        return;
    }

    const std::string display_name = entity_display_name(context.scene, entity);
    const std::vector<entt::entity> children = scene_hierarchy::get_children(context.scene, entity);
    const bool is_selected = std::find(
        context.selected_entities.begin(),
        context.selected_entities.end(),
        entity
    ) != context.selected_entities.end();

    HierarchyRenameState& rename_state = hierarchy_rename_state();
    const bool is_renaming = rename_state.entity == entity;
    const bool is_scene_root = registry.all_of<SceneRootComponent>(entity);

    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
        | ImGuiTreeNodeFlags_OpenOnDoubleClick
        | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (is_selected) {
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (children.empty()) {
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    ImGui::PushID(static_cast<int>(entt::to_integral(entity)));

    const bool node_open = is_renaming
        ? ImGui::TreeNodeEx("##renaming_node", node_flags, "%s", "")
        : ImGui::TreeNodeEx("##node", node_flags, "%s", display_name.c_str());

    if (is_renaming) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (rename_state.focus_next_frame) {
            ImGui::SetKeyboardFocusHere();
            rename_state.focus_next_frame = false;
        }

        ImGuiInputTextFlags rename_flags =
            ImGuiInputTextFlags_AutoSelectAll |
            ImGuiInputTextFlags_EnterReturnsTrue;

        if (ImGui::InputText("##rename_input", rename_state.buffer.data(), rename_state.buffer.size(), rename_flags)) {
            commit_hierarchy_rename(context);
        } else if (ImGui::IsItemDeactivatedAfterEdit()) {
            commit_hierarchy_rename(context);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            cancel_hierarchy_rename();
        }
    }

    if (!is_renaming && ImGui::IsItemClicked()) {
        if (context.on_select_entity) {
            context.on_select_entity(entity);
        }
    }

    if (!is_renaming && is_selected && ImGui::IsKeyPressed(ImGuiKey_F2)) {
        begin_hierarchy_rename(context, entity);
    }

    if (!is_renaming && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        begin_hierarchy_rename(context, entity);
    }

    if (!is_renaming && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload(k_hierarchy_drag_payload, &entity, sizeof(entt::entity));
        ImGui::TextUnformatted(display_name.c_str());
        ImGui::EndDragDropSource();
    }

    draw_hierarchy_drag_drop_targets(context, entity);

    if (ImGui::BeginPopupContextItem()) {
        if (!is_scene_root && ImGui::MenuItem("Rename") && context.on_rename_entity) {
            begin_hierarchy_rename(context, entity);
        }
        if (ImGui::MenuItem("Create Empty Child") && context.on_create_empty_node) {
            context.on_create_empty_node(entity);
        }
        if (!is_scene_root && ImGui::MenuItem("Delete") && context.on_delete_entity) {
            context.on_delete_entity(entity);
        }
        ImGui::EndPopup();
    }

    if (node_open && !children.empty()) {
        for (const entt::entity child : children) {
            draw_hierarchy_node(context, child);
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

} // namespace

inline void draw_hierarchy_panel(const char* title, bool& show_panel, const EditorUIContext& context) {
    if (!show_panel) {
        return;
    }

    if (!ImGui::Begin(title, &show_panel)) {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Create Empty")) {
        if (context.on_create_empty_node) {
            const entt::entity parent = context.selected_entities.empty()
                ? scene_hierarchy::ensure_root(context.scene)
                : context.selected_entities.front();
            context.on_create_empty_node(parent);
        }
    }

    ImGui::Separator();

    const entt::entity root = scene_hierarchy::ensure_root(context.scene);
    draw_hierarchy_node(context, root);

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
        if (context.on_clear_selection) {
            context.on_clear_selection();
        }
    }

    ImGui::End();
}

} // namespace editor_ui

#endif
