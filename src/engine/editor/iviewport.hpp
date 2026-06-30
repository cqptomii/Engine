//
// Created by tomfr on 12/03/2026.
//

#ifndef ENGINE_IVIEWPORT_HPP
#define ENGINE_IVIEWPORT_HPP

#include "engine/scene/Scene.hpp"
#include "engine/editor/editor_camera.hpp"
class IViewport
{
public:
    virtual ~IViewport() = default;
    virtual void update(Scene& scene) = 0;
    virtual EditorCamera& get_main_camera() = 0;
};

#endif //ENGINE_IVIEWPORT_HPP