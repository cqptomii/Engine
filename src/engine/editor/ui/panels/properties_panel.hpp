/**
 * @file properties_panel.hpp
 * @author Tom FRAISSE
 * @brief Header file for the properties panel
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
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

/**
 * @brief Method to draw the properties panel and its contents onto the ImGui window
 * 
 * @param title The title of the properties panel
 * @param show_panel A reference to the boolean that controls whether the properties panel is shown
 * @param context The editor UI context
 * @return void
 */
inline void draw_properties_panel(const char* title, bool& show_panel, const EditorUIContext& context) {
    // Check if the properties panel is not shown
    if (!show_panel) {
        return;
    }

    // Check if the properties panel is opened
    if (!ImGui::Begin(title, &show_panel)) {
        ImGui::End();
        return;
    }

    // Check if no object is selected
    if (context.selected_entities.empty()) {
        ImGui::TextDisabled("No object selected.");
        ImGui::End();
        return;
    }

    // Get the first selected entity
    const entt::entity entity = context.selected_entities.front();
    auto& registry = context.scene.get_registry().raw();

    // Check if the entity is valid
    if (!registry.valid(entity)) {
        ImGui::TextDisabled("Selected entity is invalid.");
        ImGui::End();
        return;
    }

    // Check if the entity has a debug name component
    if (registry.all_of<DebugNameComponent>(entity)) {
        ImGui::Text("Name: %s", registry.get<DebugNameComponent>(entity).get_name().c_str());
    }

    // Check if the entity has a transform component
    if (!registry.all_of<TransformComponent>(entity)) {
        ImGui::TextDisabled("No TransformComponent on selected entity.");
        ImGui::End();
        return;
    }

    // Get the transform component
    TransformComponent& transform = registry.get<TransformComponent>(entity);

    // Get the position, scale and rotation of the transform component
    glm::vec3 position = transform.get_position();
    glm::vec3 scale = transform.get_scale();
    glm::vec3 rotation_euler = glm::degrees(glm::eulerAngles(transform.get_rotation()));

    // Draw the transform component
    ImGui::SeparatorText("Transform");
    ImGui::TextDisabled("Drag to scrub, or click a value and type.");

    // Draw the position of the transform component
    if (ImGui::DragFloat3("Position", &position.x, 0.05f, 0.0f, 0.0f, "%.3f")) {
        transform.set_position(position);
    }

    // Draw the rotation of the transform component
    if (ImGui::DragFloat3("Rotation", &rotation_euler.x, 0.5f, 0.0f, 0.0f, "%.3f")) {
        transform.set_rotation(glm::quat(glm::radians(rotation_euler)));
    }

    // Draw the scale of the transform component
    if (ImGui::DragFloat3("Scale", &scale.x, 0.02f, 0.01f, 100.0f, "%.3f")) {
        transform.set_scale(scale);
    }

    // Check if multiple objects are selected
    if (context.selected_entities.size() > 1) {
        ImGui::Separator();
        ImGui::TextDisabled("%zu objects selected. Showing first only.", context.selected_entities.size());
    }

    // End the properties panel
    ImGui::End();
}

} // namespace editor_ui

#endif
