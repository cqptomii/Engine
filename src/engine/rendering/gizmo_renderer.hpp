#ifndef GIZMO_RENDERER_HPP
#define GIZMO_RENDERER_HPP

#include <algorithm>
#include <array>
#include <filesystem>
#include <string>
#include <vector>
#include "engine/resources/gpu/wrappers/Buffer.hpp"
#include "engine/resources/gpu/wrappers/VAO.hpp"
#include "engine/rendering/utils/transform_data.hpp"
#include "engine/resources/gpu/shader.hpp"
#include "engine/resources/cpu/shader_resource.hpp"
#include "engine/rendering/utils/camera_data.hpp"
#include "engine/core/utils.hpp"

class GizmoRenderer {

private:

    VAO gizmo_vao;
    Buffer gizmo_vbo;
    Buffer gizmo_ssbo;
    std::vector<TransformData> gizmo_transforms;
    std::string axes_vs_path;
    std::string axes_fs_path;
    size_t ssbo_capacity_bytes = 0;

    void load_gizmo_shader(){
        // Gizmo vertices position and color 
        const std::array<float, 36> axis_vertices = {
            // X axis (red)
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            // Y axis (green)
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            // Z axis (blue)
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };
            
        // Set up the VAO and VBO for the axes
        gizmo_vao.bind();
        gizmo_vbo.bind();
        gizmo_vbo.set_data(sizeof(axis_vertices), axis_vertices.data(), GL_STATIC_DRAW);

        // Link the vertices position and color attributes to the vertex shader inputs locations
        gizmo_vao.set_vertex_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)nullptr);
        gizmo_vao.set_vertex_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        gizmo_vao.unbind();
        gizmo_vbo.unbind();
    }
    void load_gizmo_ssbo(const std::vector<TransformData>& gizmo_transforms){
        ssbo_capacity_bytes = gizmo_transforms.size() * sizeof(TransformData);
        gizmo_ssbo.bind();
        gizmo_ssbo.set_data(gizmo_transforms.size() * sizeof(TransformData), gizmo_transforms.data(), GL_DYNAMIC_DRAW);
        gizmo_ssbo.unbind();
    }

public:

    GizmoRenderer() : gizmo_vao(), gizmo_vbo(GL_ARRAY_BUFFER), gizmo_ssbo(GL_SHADER_STORAGE_BUFFER){
        this->load_gizmo_shader();
        this->load_gizmo_ssbo(this->gizmo_transforms);

        // Set the paths to the vertex and fragment shaders for the gizmo axes
        this->axes_vs_path = resolve_shader_path("assets/shaders/editor/editor_axes.vs");
        this->axes_fs_path = resolve_shader_path("assets/shaders/editor/editor_axes.fs");
    }
        
    ~GizmoRenderer() = default;

    void upload_gizmo(const std::vector<TransformData>& transform){
        // Add the new gizmo transform data to the existing vector
        this->gizmo_transforms = transform;

        if (this->gizmo_transforms.empty())
        {
            return;
        }

        const size_t required_bytes = this->gizmo_transforms.size() * sizeof(TransformData);

        // Reallocate SSBO when needed; updateData is ignored if requested size exceeds current buffer size.
        if (required_bytes > this->ssbo_capacity_bytes)
        {
            this->load_gizmo_ssbo(this->gizmo_transforms);
            return;
        }

        // Update the SSBO with the new gizmo transform data
        this->gizmo_ssbo.update_data(0, required_bytes, this->gizmo_transforms.data());
    }
    void render(const CameraData& camera_data){
        static ShaderResource axes_shader_resource(axes_vs_path.c_str(), axes_fs_path.c_str());
        static Shader axes_shader(
            axes_shader_resource.get_vertex_source().c_str(),
            axes_shader_resource.get_fragment_source().c_str()
        );

        // Set up the configuration of the OpenGL state to render the gizmo axes, and restore it at the end of the function
        const GLboolean was_depth_test = glIsEnabled(GL_DEPTH_TEST);
        const GLboolean was_cull_face = glIsEnabled(GL_CULL_FACE);
        const GLboolean was_blend = glIsEnabled(GL_BLEND);
        GLfloat previous_line_width = 1.0f;
        glGetFloatv(GL_LINE_WIDTH, &previous_line_width);
        GLint previous_viewport[4] = {0, 0, 0, 0};
        glGetIntegerv(GL_VIEWPORT, previous_viewport);


        const int viewport_width = previous_viewport[2];
        const int viewport_height = previous_viewport[3];

        // Process the gizmo rendered into the top right corner of the viewport
        const int gizmo_size = std::max(64, std::min(viewport_width, viewport_height) / 6);
        const int margin = 16;
        const int gizmo_x = previous_viewport[0] + viewport_width - gizmo_size - margin;
        const int gizmo_y = previous_viewport[1] + viewport_height - gizmo_size - margin;
        glViewport(gizmo_x, gizmo_y, gizmo_size, gizmo_size);


        // Set up the OpenGL state for rendering the gizmo axes
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(2.5f);

        axes_shader.use();
        // Set the alpha value for the gizmo axes
        axes_shader.set_float("uAlpha", 1.0f);

        // Bind the gizmo VAO and SSBO, and set the camera data uniform
        gizmo_vao.bind();
        gizmo_ssbo.bind_base(0);
        glDrawArraysInstanced(GL_LINES, 0, 6, static_cast<GLsizei>(gizmo_transforms.size()));
        gizmo_vao.unbind();

        Shader::unuse();
        glViewport(previous_viewport[0], previous_viewport[1], previous_viewport[2], previous_viewport[3]);
        glLineWidth(previous_line_width);

        if (was_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if (was_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (was_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);

    }
};

#endif // GIZMO_RENDERER_HPP