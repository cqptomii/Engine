#ifndef VIEWPORT_FRAMEBUFFER_HPP
#define VIEWPORT_FRAMEBUFFER_HPP

#include <glad/glad.h>

#include <algorithm>
#include <iostream>

class ViewportFramebuffer {
    GLuint framebuffer_id = 0;
    GLuint color_texture_id = 0;
    GLuint depth_renderbuffer_id = 0;
    int width = 0;
    int height = 0;

    void destroy() {
        if (this->depth_renderbuffer_id != 0) {
            glDeleteRenderbuffers(1, &this->depth_renderbuffer_id);
            this->depth_renderbuffer_id = 0;
        }

        if (this->color_texture_id != 0) {
            glDeleteTextures(1, &this->color_texture_id);
            this->color_texture_id = 0;
        }

        if (this->framebuffer_id != 0) {
            glDeleteFramebuffers(1, &this->framebuffer_id);
            this->framebuffer_id = 0;
        }

        this->width = 0;
        this->height = 0;
    }

    void create_color_texture(const int texture_width, const int texture_height) {
        glGenTextures(1, &this->color_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->color_texture_id);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            texture_width,
            texture_height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void create_depth_renderbuffer(const int texture_width, const int texture_height) {
        glGenRenderbuffers(1, &this->depth_renderbuffer_id);
        glBindRenderbuffer(GL_RENDERBUFFER, this->depth_renderbuffer_id);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            texture_width,
            texture_height
        );
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

public:
    ViewportFramebuffer() = default;

    ~ViewportFramebuffer() {
        this->destroy();
    }

    ViewportFramebuffer(const ViewportFramebuffer&) = delete;
    ViewportFramebuffer& operator=(const ViewportFramebuffer&) = delete;

    void resize(const int new_width, const int new_height) {
        const int clamped_width = std::max(1, new_width);
        const int clamped_height = std::max(1, new_height);

        if (clamped_width == this->width && clamped_height == this->height) {
            return;
        }

        this->destroy();

        this->width = clamped_width;
        this->height = clamped_height;

        this->create_color_texture(this->width, this->height);
        this->create_depth_renderbuffer(this->width, this->height);

        glGenFramebuffers(1, &this->framebuffer_id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer_id);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            this->color_texture_id,
            0
        );
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            this->depth_renderbuffer_id
        );

        const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Viewport framebuffer is incomplete (status=" << status << ")" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer_id);
        glViewport(0, 0, this->width, this->height);
    }

    void unbind(const int default_width, const int default_height) const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, default_width, default_height);
    }

    [[nodiscard]] GLuint get_color_texture_id() const noexcept {
        return this->color_texture_id;
    }

    [[nodiscard]] int get_width() const noexcept {
        return this->width;
    }

    [[nodiscard]] int get_height() const noexcept {
        return this->height;
    }

    [[nodiscard]] bool is_valid() const noexcept {
        return this->framebuffer_id != 0 && this->color_texture_id != 0;
    }
};

#endif
