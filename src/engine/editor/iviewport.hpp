//
// Created by tomfr on 12/03/2026.
//

#ifndef ENGINE_IVIEWPORT_HPP
#define ENGINE_IVIEWPORT_HPP

#include "../src/engine/core/input/input_manager.hpp"
#include "../src/engine/scene/Scene.hpp"

class IViewport
{
public:
    virtual ~IViewport() = default;
    virtual void update(Scene& scene, InputManager& input_manager);
    virtual EditorCamera& get_main_camera();
};

#endif //ENGINE_IVIEWPORT_HPP