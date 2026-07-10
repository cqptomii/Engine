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
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

#include <entt/entt.hpp>
#include "engine/core/event/event/action_performed_event.hpp"
#include "engine/core/event/event/ievent.hpp"
#include "engine/core/event/event_listener.hpp"
#include "engine/core/event/event_bus.hpp"
#include "engine/core/event/event_dispatcher.hpp"
#include "engine/core/event/event_category.hpp"
#include "engine/core/event/event/action_started_event.hpp"
#include "engine/core/event/event/action_ended_event.hpp"
#include "engine/core/event/event/mouse_delta_event.hpp"
#include "engine/core/event/event/mouse_scroll_event.hpp"
#include "engine/core/event/event/window_resize_event.hpp"
#include "engine/core/input/input_manager.hpp"

#include "engine/editor/picking.hpp"
#include "engine/editor/manipulation_mode.hpp"
#include "engine/editor/object_manipulation.hpp"


class EditorViewport : public IViewport, public EventListener{

    // Editor Camera Object
    EditorCamera editor_camera;

    // Activate the verbose mode
    bool verbose = false;

    EventBus& event_bus;

    // Input Manager Object
    InputManager& input_manager;

    // Active actions
    std::unordered_set<std::string> active_actions;

    // Framebuffer Size
    glm::vec2 framebuffer_size = glm::vec2(800, 600);

    // Selected objects
    std::vector<entt::entity> selected_objects;

    // Picking Mode events flags
    bool pick_one_object = false;
    bool pick_multiple_objects = false;
    bool pick_all_objects = false;

    // Manipulation Mode
    bool manipulation_hold = false;

    // Pending manipulation object to apply the manipulation to the selected objects
    PendingManipulation pending_manipulation;


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
     * @param input_manager (InputManager&) : InputManager reference
     */
    EditorViewport(EventBus& bus, InputManager& input_manager) : editor_camera(EditorCamera()), event_bus(bus), input_manager(input_manager)    
    {
        // Show cam Position
        this->editor_camera.debug_cam();

        // Listen Input and Mouse Events
        event_bus.add_listener(this,
            static_cast<int>(EventCategory::Input) |
            static_cast<int>(EventCategory::Mouse) |
            static_cast<int>(EventCategory::Window)
        );
    }

    /**
     * @brief Construct a new Editor Viewport object
     * 
     * @param bus (EventBus&) : EventBus reference
     * @param input_manager (InputManager&) : InputManager reference
     * @param camera (EditorCamera&) : EditorCamera reference
     */
    explicit EditorViewport(EventBus& bus, InputManager& input_manager, const EditorCamera& camera) : editor_camera(camera), event_bus(bus), input_manager(input_manager){
        // Show cam Position
        this->editor_camera.debug_cam();

        // Listen Input and Mouse Events
        event_bus.add_listener(this,
            static_cast<int>(EventCategory::Input) |
            static_cast<int>(EventCategory::Mouse) |
            static_cast<int>(EventCategory::Window)
        );
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

            // Process Picking event to get the object under the mouse cursor
            if( e.get_action_name() == "pick_one_object") {
                pick_one_object = true;
            }
            else if( e.get_action_name() == "pick_multiple_objects") {
                pick_multiple_objects = true;
            }
            else if( e.get_action_name() == "pick_all_objects") {
                pick_all_objects = true;
            }

            // Set the manipulation mode
            if (e.get_action_name() == "manipulation_mode_translate"){
                pending_manipulation.mode = ManipulationMode::TRANSLATE;
            }
            else if (e.get_action_name() == "manipulation_mode_rotate"){
                pending_manipulation.mode = ManipulationMode::ROTATE;
            }
            else if (e.get_action_name() == "manipulation_mode_scale"){
                pending_manipulation.mode = ManipulationMode::SCALE_UNIFORM;
            }else if (e.get_action_name() == "manipulation_mode_none"){
                pending_manipulation.mode = ManipulationMode::NONE;
            }
        });

        // Process each ActionPerformed listened bu the EditorViewport
        dispatcher.dispatch<ActionPerformedEvent>([this](const ActionPerformedEvent& e) -> void {
            active_actions.insert(e.get_action_name());
            
            // Move the camera with the Keyboard Mappings
            if (e.get_action_name() == "camera_move_left"){
                editor_camera.process_cam_movement(CameraMovement::LEFT);
            }
            if (e.get_action_name() == "camera_move_right"){
                editor_camera.process_cam_movement(CameraMovement::RIGHT);
            }
            if (e.get_action_name() == "camera_move_top"){
                editor_camera.process_cam_movement(CameraMovement::TOP);
            }
            if (e.get_action_name() == "camera_move_bottom"){
                editor_camera.process_cam_movement(CameraMovement::BOTTOM);
            }


            // Set the manipulation hold
            if (e.get_action_name() == "manipulation_mode_translate"){
                manipulation_hold = true;
            }
            else if (e.get_action_name() == "manipulation_mode_rotate"){
                manipulation_hold = true;
            }
            else if (e.get_action_name() == "manipulation_mode_scale"){
                manipulation_hold = true;
            }
        });

        // Process each ActionEndedEvent listened by the EditorViewport
        dispatcher.dispatch<ActionEndedEvent>([this](const ActionEndedEvent& e) {
            
            // Reset the manipulation hold
            if (e.get_action_name() == "manipulation_mode_translate"){
                manipulation_hold = false;
                pending_manipulation.is_active = false;
            }
            else if (e.get_action_name() == "manipulation_mode_rotate"){
                manipulation_hold = false;  
                pending_manipulation.is_active = false;
            }
            else if (e.get_action_name() == "manipulation_mode_scale"){
                manipulation_hold = false;
                pending_manipulation.is_active = false;
            }


            active_actions.erase(e.get_action_name());
        });


        // Process each MouseDeltaEvent consumed to update the Camera position and rotation
        dispatcher.dispatch<MouseDeltaEvent>([this](const MouseDeltaEvent& e) {
            if (active_actions.contains("camera_vector_move")) {
                editor_camera.process_cam_movement(e.get_x_offset(), e.get_y_offset());
            }
            else if (active_actions.contains("rotate_camera")) {
                editor_camera.process_cam_rotation(e.get_x_offset(), e.get_y_offset(), 0.f);
            }else if( manipulation_hold && !selected_objects.empty()){
                if (active_actions.contains("manipulation_mode_translate")
                    || active_actions.contains("manipulation_mode_rotate")
                    || active_actions.contains("manipulation_mode_scale"))
                {
                    pending_manipulation.delta_x += e.get_x_offset();
                    pending_manipulation.delta_y += e.get_y_offset();
                    pending_manipulation.is_active = true;
                }
            }
        });

        // Process MouseScrolledEvent to update the camera FOV
        dispatcher.dispatch<MouseScrollEvent>([this](const MouseScrollEvent& e) {
            editor_camera.process_cam_zoom(e.get_y_offset());
        });

        // Process WindowResizeEvent to update the framebuffer size
        dispatcher.dispatch<WindowResizeEvent>([this](const WindowResizeEvent& e) {
            framebuffer_size.x = e.get_width();
            framebuffer_size.y = e.get_height();
        });
    }

    /**
     * @brief Set the framebuffer size used for screen-to-world picking.
     */
    void set_framebuffer_size(const int width, const int height)
    {
        this->framebuffer_size = glm::vec2(static_cast<float>(width), static_cast<float>(height));
    }

    /**
     * @brief Update the EditorViewport
     * 
     * @param scene (Scene&) : Scene reference
     * @details Update the EditorViewport and process the picking event if needed
     */
    void update(Scene& scene) override
    {
        // Process the picking event if needed
        if (pick_one_object || pick_multiple_objects){
            glm::vec2 mouse_position = input_manager.get_mouse_position();

            // Create a ray from the camera to the mouse position
            Ray ray = editor_camera.screen_point_to_ray(mouse_position.x, mouse_position.y, framebuffer_size.x, framebuffer_size.y);

            // Pick the closest object to the ray
            PickingResult object_picked = pick_closest_entity(scene, ray);

            // If the picking result is a hit, add the entity to the selected objects
            if (object_picked.hit){

                if(verbose){
                    std::cout << "Object picked: " << entt::to_integral(object_picked.entity)
                              << " at distance " << object_picked.distance << std::endl;
                }
                if(pick_one_object){
                    selected_objects = { object_picked.entity };
                }else if(pick_multiple_objects){

                    // If the object is not already in the selected objects, add it
                    if(std::find(selected_objects.begin(), selected_objects.end(), object_picked.entity) == selected_objects.end()){
                        selected_objects.push_back(object_picked.entity);
                    }
                }
            }else{

                if(verbose){
                    std::cout << "No object picked" << std::endl;
                }

                // If no object is picked, clear the selected objects
                if(pick_one_object){
                    selected_objects.clear();
                }
            }

            // Reset the picking mode flags
            pick_one_object = false;
            pick_multiple_objects = false;
            pick_all_objects = false;
        }else if (pick_all_objects){
            // Pick all entities in the scene
            std::vector<entt::entity> all_entities = pick_all_entities(scene);
            // Set the selected objects to the all entities
            selected_objects = all_entities;
            // Reset the picking mode flags
            pick_all_objects = false;
        }
        

        // Process the pending manipulation if needed
        if (pending_manipulation.is_active && !selected_objects.empty()){
            
            apply_manipulation(scene, selected_objects, pending_manipulation.mode, pending_manipulation.delta_x, pending_manipulation.delta_y);
            
            pending_manipulation.is_active = false;
            pending_manipulation.delta_x = 0.0f;
            pending_manipulation.delta_y = 0.0f;
        }

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


    /**
     * @brief Get the selected objects
     * 
     * @return std::vector<entt::entity> : Vector of selected objects
     */
    std::vector<entt::entity> get_selected_objects() const
    {
        return this->selected_objects;
    }

    /**
     * @brief Get the pending manipulation object
     * 
     * @return const PendingManipulation& : PendingManipulation object
     */
    const PendingManipulation& get_pending_manipulation() const{
        return this->pending_manipulation;
    }

    /**
     * @brief Get the manipulation mode
     * 
     * @return ManipulationMode : ManipulationMode enum value
     */
    const ManipulationMode& get_manipulation_mode() const{
        return this->pending_manipulation.mode;
    }
};



#endif //EDITOR_VIEWPORT_HPP
