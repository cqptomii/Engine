#ifndef GRID_RENDERER_HPP
#define GRID_RENDERER_HPP

#include "engine/resources/gpu/wrappers/Buffer.hpp"
#include "engine/resources/gpu/wrappers/VAO.hpp"
#include "engine/resources/gpu/shader.hpp"
#include "engine/resources/cpu/shader_resource.hpp"
#include "engine/rendering/utils/camera_data.hpp"
#include "engine/core/utils.hpp"
#include "engine/core/debug/debug_hooks.hpp"


class GridRenderer {
private:
    VAO grid_vao;
    Buffer grid_vbo;
    std::string grid_vs_path;
    std::string grid_fs_path;

    void load_grid_shader(){
        grid_vs_path = resolve_shader_path("assets/shaders/editor/editor_grid.vs");
        grid_fs_path = resolve_shader_path("assets/shaders/editor/editor_grid.fs");

        const std::array<float, 18> quad_vertices = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f
        };

        // Set up the VAO and VBO for the grid
        grid_vao.bind();
        grid_vbo.bind();

        grid_vbo.set_data(sizeof(quad_vertices), quad_vertices.data(), GL_STATIC_DRAW);

        // Link the vertices position to the vertex shader input location
        grid_vao.set_vertex_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        grid_vao.unbind();
        grid_vbo.unbind();
    }
public:

    GridRenderer() : grid_vs_path(""), grid_fs_path(""), grid_vao(), grid_vbo(GL_ARRAY_BUFFER) {
        load_grid_shader();
    }
    ~GridRenderer() = default;

    void render(const CameraData& camera_data){
        static ShaderResource grid_shader_resource(grid_vs_path.c_str(), grid_fs_path.c_str());
        static Shader grid_shader(
            grid_shader_resource.get_vertex_source().c_str(),
            grid_shader_resource.get_fragment_source().c_str()
        );

        const GLboolean was_depth_test = glIsEnabled(GL_DEPTH_TEST);
        const GLboolean was_cull_face = glIsEnabled(GL_CULL_FACE);
        const GLboolean was_blend = glIsEnabled(GL_BLEND);
        GLboolean was_depth_write = GL_TRUE;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &was_depth_write);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        const glm::mat4 view_proj = camera_data.projection * camera_data.view;
        const glm::mat4 inv_view_proj = glm::inverse(view_proj);

        grid_shader.use();
        grid_shader.set_mat4("uViewProj", view_proj);
        grid_shader.set_mat4("uInvViewProj", inv_view_proj);
        grid_shader.set_vec3("uCameraPos", camera_data.position);

        // Default editor grid tuning values.
        grid_shader.set_float("uMinorStep", 1.0f);
        grid_shader.set_float("uMajorEvery", 10.0f);
        grid_shader.set_float("uLineThickness", 0.2f);
        grid_shader.set_float("uFadeStart", 180.0f);
        grid_shader.set_float("uFadeEnd", 420.0f);
        grid_shader.set_float("uGridHeight", -0.01f);
        grid_shader.set_vec4("uMinorColor", glm::vec4(0.35f, 0.35f, 0.37f, 0.45f));
        grid_shader.set_vec4("uMajorColor", glm::vec4(0.52f, 0.52f, 0.55f, 0.65f));

        grid_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        debug_record_raw_draw_call();
        grid_vao.unbind();

        Shader::unuse();

        // Restore the previous OpenGL state
        if (!was_depth_test) glDisable(GL_DEPTH_TEST); else glEnable(GL_DEPTH_TEST);
        if (was_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (was_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        glDepthMask(was_depth_write);
    }
};

#endif // GRID_RENDERER_HPP