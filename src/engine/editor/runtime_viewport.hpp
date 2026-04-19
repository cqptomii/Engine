//
// Created by tomfr on 06/03/2026.
//

#ifndef RUNTIME_VIEWPORT_HPP
#define RUNTIME_VIEWPORT_HPP

#include "engine/editor/iviewport.hpp"
#include "engine/editor/editor_camera.hpp"

class RuntimeViewport final : public IViewport
{
	EditorCamera runtime_camera;

public:
	RuntimeViewport() = default;
	~RuntimeViewport() override = default;

	void update(Scene& scene, InputManager& input_manager) override
	{
		(void)scene;
		(void)input_manager;
	}

	EditorCamera& get_main_camera() override
	{
		return this->runtime_camera;
	}
};

#endif //RUNTIME_VIEWPORT_HPP
