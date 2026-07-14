#ifndef ENGINE_PROPERTIES_PANEL_HPP
#define ENGINE_PROPERTIES_PANEL_HPP

#include <imgui.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "engine/ecs/components/debug_name_component.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/editor/ui/editor_context.hpp"

namespace editor_ui {

inline void draw_properties_panel(const char* title, bool& show_panel, const EditorUIContext& context) {
    if (!show_panel) {
        return;
    }

    if (!ImGui::Begin(title, &show_panel)) {
        ImGui::End();
        return;
    }

    if (context.selected_entities.empty()) {
        ImGui::TextDisabled("No object selected.");
        ImGui::End();
        return;
    }

    const entt::entity entity = context.selected_entities.front();
    auto& registry = context.scene.get_registry().raw();

    if (!registry.valid(entity)) {
        ImGui::TextDisabled("Selected entity is invalid.");
        ImGui::End();
        return;
    }

    if (registry.all_of<DebugNameComponent>(entity)) {
        ImGui::Text("Name: %s", registry.get<DebugNameComponent>(entity).get_name().c_str());
    }

    if (!registry.all_of<TransformComponent>(entity)) {
        ImGui::TextDisabled("No TransformComponent on selected entity.");
        ImGui::End();
        return;
    }

    TransformComponent& transform = registry.get<TransformComponent>(entity);

    glm::vec3 position = transform.get_position();
    glm::vec3 scale = transform.get_scale();
    glm::vec3 rotation_euler = glm::degrees(glm::eulerAngles(transform.get_rotation()));

    ImGui::SeparatorText("Transform");
    ImGui::TextDisabled("Drag to scrub, or click a value and type.");

    if (ImGui::DragFloat3("Position", &position.x, 0.05f, 0.0f, 0.0f, "%.3f")) {
        transform.set_position(position);
    }

    if (ImGui::DragFloat3("Rotation", &rotation_euler.x, 0.5f, 0.0f, 0.0f, "%.3f")) {
        transform.set_rotation(glm::quat(glm::radians(rotation_euler)));
    }

    if (ImGui::DragFloat3("Scale", &scale.x, 0.02f, 0.01f, 100.0f, "%.3f")) {
        transform.set_scale(scale);
    }

    if (context.selected_entities.size() > 1) {
        ImGui::Separator();
        ImGui::TextDisabled("%zu objects selected. Showing first only.", context.selected_entities.size());
    }

    ImGui::End();
}

} // namespace editor_ui

#endif
