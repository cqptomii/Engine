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

#include "engine/editor/picking/picking.hpp"
#include "engine/editor/picking/manipulation_mode.hpp"
#include "engine/editor/picking/object_manipulation.hpp"
#include "engine/editor/picking/selection_context.hpp"
#include "engine/editor/gizmo/gizmo_axis.hpp"
#include "engine/editor/gizmo/gizmo_picker.hpp"
#include "engine/editor/gizmo/gizmo_manipulator.hpp"
#include "engine/rendering/utils/camera_data.hpp"
#include "engine/editor/ui/viewport_layout.hpp"


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

    // Viewport render target size (FBO / picking projection)
    glm::vec2 viewport_render_size = glm::vec2(800.0f, 600.0f);

    // Viewport panel bounds in GLFW client coordinates
    ViewportClientBounds viewport_client_bounds{};

    // Whether mouse interactions should affect the 3D viewport this frame
    bool viewport_input_enabled = true;

    // Whether editor keyboard shortcuts should affect the viewport this frame
    bool viewport_keyboard_enabled = true;

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

    // Gizmo drag state (LMB grab on an axis handle while a mode is active)
    bool gizmo_drag_active = false;
    GizmoAxis active_gizmo_axis = GizmoAxis::NONE;
    float gizmo_delta_x = 0.0f;
    float gizmo_delta_y = 0.0f;

    // Rotation drag state: last ring-plane direction (from pivot) used to
    // measure the swept angle between two mouse positions.
    glm::vec3 gizmo_rot_prev_dir = glm::vec3(0.0f);
    bool gizmo_rot_has_prev = false;


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

            if (e.get_action_name() == "reset_camera") {
                if (!this->viewport_keyboard_enabled) {
                    return;
                }
                editor_camera.reset();
                return;
            }

            if (e.get_action_name() == "pick_one_object"
                || e.get_action_name() == "pick_multiple_objects"
                || e.get_action_name() == "pick_all_objects") {
                if (!this->viewport_input_enabled) {
                    return;
                }
            }

            if (e.get_action_name() == "manipulation_mode_translate"
                || e.get_action_name() == "manipulation_mode_rotate"
                || e.get_action_name() == "manipulation_mode_scale"
                || e.get_action_name() == "manipulation_mode_none") {
                if (!this->viewport_keyboard_enabled) {
                    return;
                }
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

            if (e.get_action_name() == "camera_move_left"
                || e.get_action_name() == "camera_move_right"
                || e.get_action_name() == "camera_move_top"
                || e.get_action_name() == "camera_move_bottom") {
                if (!this->viewport_keyboard_enabled) {
                    return;
                }
            }

            if (e.get_action_name() == "manipulation_mode_translate"
                || e.get_action_name() == "manipulation_mode_rotate"
                || e.get_action_name() == "manipulation_mode_scale") {
                if (!this->viewport_keyboard_enabled) {
                    return;
                }
            }
            
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

            // End of a gizmo drag when the left mouse button is released.
            if (e.get_action_name() == "pick_one_object"){
                gizmo_drag_active = false;
                active_gizmo_axis = GizmoAxis::NONE;
                gizmo_rot_has_prev = false;
            }


            active_actions.erase(e.get_action_name());
        });


        // Process each MouseDeltaEvent consumed to update the Camera position and rotation
        dispatcher.dispatch<MouseDeltaEvent>([this](const MouseDeltaEvent& e) {
            if (!this->viewport_input_enabled) {
                return;
            }

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
            }else if (gizmo_drag_active && !selected_objects.empty()){
                // Dragging a grabbed gizmo axis with the left mouse button.
                gizmo_delta_x += e.get_x_offset();
                gizmo_delta_y += e.get_y_offset();
            }
        });

        // Process MouseScrolledEvent to update the camera FOV
        dispatcher.dispatch<MouseScrollEvent>([this](const MouseScrollEvent& e) {
            if (!this->viewport_input_enabled) {
                return;
            }

            editor_camera.process_cam_zoom(e.get_y_offset());
        });

        dispatcher.dispatch<WindowResizeEvent>([this](const WindowResizeEvent& e) {
            this->viewport_render_size = glm::vec2(static_cast<float>(e.get_width()), static_cast<float>(e.get_height()));
        });
    }

    [[nodiscard]] glm::vec2 get_viewport_local_mouse() const {
        const glm::vec2 client_mouse = this->input_manager.get_mouse_position();
        return viewport_client_to_framebuffer(
            client_mouse,
            this->viewport_client_bounds,
            this->viewport_render_size
        );
    }

    /**
     * @brief Configure viewport input gating and client bounds (previous frame).
     */
    void set_viewport_input_context(
        const ViewportClientBounds& bounds,
        const bool mouse_input_enabled,
        const bool keyboard_input_enabled)
    {
        this->viewport_client_bounds = bounds;
        this->viewport_input_enabled = mouse_input_enabled;
        this->viewport_keyboard_enabled = keyboard_input_enabled;
    }

    void select_entity(const entt::entity entity) {
        if (entity == entt::null) {
            this->selected_objects.clear();
            return;
        }

        this->selected_objects = { entity };
    }

    void clear_selection() {
        this->selected_objects.clear();
    }

    /**
     * @brief Set the viewport render target size used for projection and picking.
     */
    void set_viewport_render_size(const int width, const int height) {
        this->viewport_render_size = glm::vec2(static_cast<float>(width), static_cast<float>(height));
    }

    /**
     * @brief Set the framebuffer size used for screen-to-world picking.
     * @deprecated Use set_viewport_render_size().
     */
    void set_framebuffer_size(const int width, const int height)
    {
        this->set_viewport_render_size(width, height);
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
        if (this->viewport_input_enabled && (pick_one_object || pick_multiple_objects)){
            const glm::vec2 local_mouse = this->get_viewport_local_mouse();
            if (local_mouse.x < 0.0f || local_mouse.y < 0.0f) {
                pick_one_object = false;
                pick_multiple_objects = false;
                pick_all_objects = false;
            } else {
            // Create a ray from the camera to the mouse position
            Ray ray = editor_camera.screen_point_to_ray(
                local_mouse.x,
                local_mouse.y,
                static_cast<int>(this->viewport_render_size.x),
                static_cast<int>(this->viewport_render_size.y)
            );

            // Gizmo has priority over object picking: with an active manipulation
            // mode, a left click that grabs an axis handle starts a gizmo drag
            // instead of (de)selecting an object.
            bool gizmo_grabbed = false;
            if (pick_one_object && pending_manipulation.mode != ManipulationMode::NONE && !selected_objects.empty()){
                const CameraData camera_data = editor_camera.get_camera_data(
                    static_cast<int>(this->viewport_render_size.x),
                    static_cast<int>(this->viewport_render_size.y));
                const SelectionContext selection_context = build_selection_context(scene, selected_objects);
                const GizmoAxis grabbed_axis = pick_gizmo_axis(ray, selection_context, pending_manipulation.mode, camera_data);

                if (grabbed_axis != GizmoAxis::NONE){
                    gizmo_drag_active = true;
                    active_gizmo_axis = grabbed_axis;
                    gizmo_delta_x = 0.0f;
                    gizmo_delta_y = 0.0f;
                    gizmo_rot_has_prev = false;
                    gizmo_grabbed = true;
                }
            }

            // Pick the closest object to the ray (skipped when a gizmo axis was grabbed)
            if (!gizmo_grabbed){
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
            }

            // Reset the picking mode flags
            pick_one_object = false;
            pick_multiple_objects = false;
            pick_all_objects = false;
            }
        }else if (pick_all_objects){
            // Pick all entities in the scene
            std::vector<entt::entity> all_entities = pick_all_entities(scene);
            // Set the selected objects to the all entities
            selected_objects = all_entities;
            // Reset the picking mode flags
            pick_all_objects = false;
        }
        

        // Process the pending manipulation if needed
        if (this->viewport_input_enabled && pending_manipulation.is_active && !selected_objects.empty()){

            const CameraData manipulation_camera = editor_camera.get_camera_data(
                static_cast<int>(this->viewport_render_size.x),
                static_cast<int>(this->viewport_render_size.y));

            apply_manipulation(scene, selected_objects, pending_manipulation.mode, pending_manipulation.delta_x, pending_manipulation.delta_y, manipulation_camera);
            
            pending_manipulation.is_active = false;
            pending_manipulation.delta_x = 0.0f;
            pending_manipulation.delta_y = 0.0f;
        }

        // Process an active gizmo axis drag (LMB grab on a handle)
        if (this->viewport_input_enabled && gizmo_drag_active && !selected_objects.empty()){

            const CameraData camera_data = editor_camera.get_camera_data(
                static_cast<int>(this->viewport_render_size.x),
                static_cast<int>(this->viewport_render_size.y));
            const SelectionContext selection_context = build_selection_context(scene, selected_objects);

            if (pending_manipulation.mode == ManipulationMode::ROTATE){
                // Rotation: measure the angle swept on the ring plane between the
                // previous and current mouse rays (accurate from any view angle).
                const glm::vec2 local_mouse = this->get_viewport_local_mouse();
                if (local_mouse.x < 0.0f || local_mouse.y < 0.0f) {
                    gizmo_delta_x = 0.0f;
                    gizmo_delta_y = 0.0f;
                } else {
                Ray ray = editor_camera.screen_point_to_ray(
                    local_mouse.x,
                    local_mouse.y,
                    static_cast<int>(this->viewport_render_size.x),
                    static_cast<int>(this->viewport_render_size.y)
                );

                glm::vec3 current_dir;
                if (gizmo_ring_plane_dir(ray, selection_context, active_gizmo_axis, current_dir)){
                    if (gizmo_rot_has_prev){
                        apply_gizmo_rotation_drag(scene, selected_objects, active_gizmo_axis, selection_context, gizmo_rot_prev_dir, current_dir);
                    }
                    gizmo_rot_prev_dir = current_dir;
                    gizmo_rot_has_prev = true;
                }

                gizmo_delta_x = 0.0f;
                gizmo_delta_y = 0.0f;
                }
            }
            else if (gizmo_delta_x != 0.0f || gizmo_delta_y != 0.0f){
                apply_gizmo_manipulation(
                    scene,
                    selected_objects,
                    pending_manipulation.mode,
                    active_gizmo_axis,
                    selection_context,
                    camera_data,
                    gizmo_delta_x,
                    gizmo_delta_y);

                gizmo_delta_x = 0.0f;
                gizmo_delta_y = 0.0f;
            }
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
