#ifndef ENGINE_HIERARCHY_PANEL_HPP
#define ENGINE_HIERARCHY_PANEL_HPP

#include <algorithm>
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

    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
        | ImGuiTreeNodeFlags_OpenOnDoubleClick
        | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (is_selected) {
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (children.empty()) {
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    const bool node_open = ImGui::TreeNodeEx(
        reinterpret_cast<void*>(static_cast<intptr_t>(entt::to_integral(entity))),
        node_flags,
        "%s",
        display_name.c_str()
    );

    if (ImGui::IsItemClicked()) {
        if (context.on_select_entity) {
            context.on_select_entity(entity);
        }
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload(k_hierarchy_drag_payload, &entity, sizeof(entt::entity));
        ImGui::TextUnformatted(display_name.c_str());
        ImGui::EndDragDropSource();
    }

    draw_hierarchy_drag_drop_targets(context, entity);

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Create Empty Child") && context.on_create_empty_node) {
            context.on_create_empty_node(entity);
        }
        const bool is_scene_root = registry.all_of<SceneRootComponent>(entity);
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
