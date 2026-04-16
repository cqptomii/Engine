//
// Created by tomfr on 06/03/2026.
//

#ifndef EDITOR_VIEWPORT_HPP
#define EDITOR_VIEWPORT_HPP


#include "editor_camera.hpp"
#include "engine/editor/iviewport.hpp"
#include "engine/core/input/input_manager.hpp"
#include "engine/scene/Scene.hpp"


class EditorViewport : public IViewport{
    EditorCamera editor_camera;
    bool verbose = false;

public:

    EditorViewport() : editor_camera(EditorCamera())
    {
        // Show cam Position
        this->editor_camera.debug_cam();
    }

    explicit EditorViewport(const EditorCamera& camera) : editor_camera(camera){}
    ~EditorViewport() override = default;

    void update(Scene& scene, InputManager& input_manager) override
    {
        std::unordered_map<std::string, uint32_t> action_mapping = input_manager.get_action_mapping();

        for (auto& [action_name, action_id] : action_mapping)
        {
            if (input_manager.is_action_active(action_id))
            {
                if (action_name == "move_camera")
                {
                    std::cout << "Move camera" << std::endl;
                    //Update cam position with the mouse movement
                    const glm::vec3 mouse_delta = {input_manager.get_mouse_offset(),0};
                    this->editor_camera.process_cam_movement(mouse_delta);

                    // Show cam Position
                    if (this->verbose)
                        this->editor_camera.debug_cam();

                }else if (action_name == "rotate_camera")
                {
                    std::cout << "Rotate camera" << std::endl;
                    const glm::vec2 mouse_delta = input_manager.get_mouse_offset();
                    std::cout << mouse_delta.x << " " << mouse_delta.y << std::endl;
                    this->editor_camera.process_cam_rotation(mouse_delta.x, mouse_delta.y, 0);

                    // Show cam Position
                    if (this->verbose)
                        this->editor_camera.debug_cam();
                }
            }
        }

        const float scroll_delta = input_manager.get_scroll_delta();
        if (scroll_delta != 0.0f)
        {
            std::cout << "Zoom camera: " << scroll_delta << std::endl;
            this->editor_camera.process_cam_zoom(scroll_delta);
            this->editor_camera.debug_cam();
        }

    }

    EditorCamera& get_main_camera() override
    {
        return this->editor_camera;
    }
};



#endif //EDITOR_VIEWPORT_HPP
