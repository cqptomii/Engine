/**
 * @file editor_viewport.hpp
 * @author Tom FRAISSE
 * @brief 
 * @version 0.1
 * @date 2026-03-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef EDITOR_VIEWPORT_HPP
#define EDITOR_VIEWPORT_HPP


#include "editor_camera.hpp"
#include "engine/editor/iviewport.hpp"
#include "engine/scene/Scene.hpp"
#include <unordered_set>
#include "engine/systems/event_system/event/action_performed_event.hpp"
#include "engine/systems/event_system/event/ievent.hpp"
#include "engine/systems/event_system/event_listener.hpp"
#include "engine/systems/event_system/event_bus.hpp"
#include "engine/systems/event_system/event_dispatcher.hpp"
#include "engine/systems/event_system/event_category.hpp"
#include "engine/systems/event_system/event/action_started_event.hpp"
#include "engine/systems/event_system/event/action_ended_event.hpp"
#include "engine/systems/event_system/event/mouse_delta_event.hpp"
#include "engine/systems/event_system/event/mouse_scroll_event.hpp"


class EditorViewport : public IViewport, public EventListener{

    // Editor Camera Object
    EditorCamera editor_camera;

    // Activate the verbose mode
    bool verbose = false;

    EventBus& event_bus;

    // Active actions
    std::unordered_set<std::string> active_actions;

public:
    /**
     * @brief Delete the default constructor to avoid EventBus duplication
     * 
     */
    EditorViewport() = delete;

    /**
     * @brief Construct a new Editor Viewport object
     * 
     * @param bus (EventBus&) : EventBus reference
     */
    EditorViewport(EventBus& bus) : editor_camera(EditorCamera()), event_bus(bus)
    {
        // Show cam Position
        this->editor_camera.debug_cam();

        // Listen Input and Mouse Events
        event_bus.add_listener(this,
            static_cast<int>(EventCategory::Input) |
            static_cast<int>(EventCategory::Mouse));
    }

    /**
     * @brief Construct a new Editor Viewport object
     * 
     * @param bus (EventBus&) : EventBus reference
     * @param camera (EditorCamera&) : EditorCamera reference
     */
    explicit EditorViewport(EventBus& bus, const EditorCamera& camera) : editor_camera(camera), event_bus(bus){
        // Show cam Position
        this->editor_camera.debug_cam();

        // Listen Input and Mouse Events
        event_bus.add_listener(this,
            static_cast<int>(EventCategory::Input) |
            static_cast<int>(EventCategory::Mouse));
    }
    

    /**
     * @brief Destructor of the current class
     * @details Remove the Editor viewport from the bus listener
     */
    ~EditorViewport() override{
        this->event_bus.remove_listener(this);
    }

    /**
     * @brief 
     * 
     * @param event 
     */
    void on_event(const IEvent& event) override{
        EventDispatcher dispatcher(event);
        
        // Process each ActionStartedEvent listened by the EditorViewport
        dispatcher.dispatch<ActionStartedEvent>([this](const ActionStartedEvent& e) {
            active_actions.insert(e.get_action_name());
            
            // Reset the camera to it's initial position
            if (e.get_action_name() == "reset_camera") {
                editor_camera.reset();
            }

            // Move the camera with the Keyboard Mappings
            // Move the camera with the Keyboard Mappings
            if (e.get_action_name() == "camera_move_left"){
                editor_camera.process_cam_movement(CameraMovement::LEFT, 1.0f);
            }
            if (e.get_action_name() == "camera_move_right"){
                editor_camera.process_cam_movement(CameraMovement::RIGHT, 1.0f);
            }
            if (e.get_action_name() == "camera_move_top"){
                editor_camera.process_cam_movement(CameraMovement::TOP, 1.0f);
            }
            if (e.get_action_name() == "camera_move_bottom"){
                editor_camera.process_cam_movement(CameraMovement::BOTTOM, 1.0f);
            }

        });

        // Process each ActionPerformed listened bu the EditorViewport
        dispatcher.dispatch<ActionPerformedEvent>([this](const ActionPerformedEvent& e) -> void {
            active_actions.insert(e.get_action_name());
            
            // Move the camera with the Keyboard Mappings
            if (e.get_action_name() == "camera_move_left"){
                editor_camera.process_cam_movement(CameraMovement::LEFT, 1.0f);
            }
            if (e.get_action_name() == "camera_move_right"){
                editor_camera.process_cam_movement(CameraMovement::RIGHT, 1.0f);
            }
            if (e.get_action_name() == "camera_move_top"){
                editor_camera.process_cam_movement(CameraMovement::TOP, 1.0f);
            }
            if (e.get_action_name() == "camera_move_bottom"){
                editor_camera.process_cam_movement(CameraMovement::BOTTOM, 1.0f);
            }
        });

        // Process each ActionEndedEvent listened by the EditorViewport
        dispatcher.dispatch<ActionEndedEvent>([this](const ActionEndedEvent& e) {
            active_actions.erase(e.get_action_name());
        });


        // Process each MouseDeltaEvent consumed to update the Camera position and rotation
        dispatcher.dispatch<MouseDeltaEvent>([this](const MouseDeltaEvent& e) {
            if (active_actions.contains("camera_vector_move")) {
                editor_camera.process_cam_movement(e.get_x_offset(), e.get_y_offset());
            }
            else if (active_actions.contains("rotate_camera")) {
                editor_camera.process_cam_rotation(e.get_x_offset(), e.get_y_offset(), 0.f);
            }
        });

        // Process MouseScrolledEvent to update the camera FOV
        dispatcher.dispatch<MouseScrollEvent>([this](const MouseScrollEvent& e) {
            editor_camera.process_cam_zoom(e.get_y_offset());
        });
    }

    void update(Scene& scene) override
    {
        (void) scene;

        if (verbose) {
            editor_camera.debug_cam();
        }
    }

    /**
     * @brief Get the main camera object of the EditorViewport
     * 
     * @return EditorCamera& : EditorCamera Object
     */
    EditorCamera& get_main_camera() override
    {
        return this->editor_camera;
    }
};



#endif //EDITOR_VIEWPORT_HPP
