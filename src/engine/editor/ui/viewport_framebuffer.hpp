/**
 * @file viewport_framebuffer.hpp
 * @author Tom FRAISSE
 * @brief Header file for the viewport framebuffer
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef VIEWPORT_FRAMEBUFFER_HPP
#define VIEWPORT_FRAMEBUFFER_HPP

#include <glad/glad.h>

#include <algorithm>
#include <iostream>

/**
 * @brief Class representing the viewport framebuffer
 */
class ViewportFramebuffer {
    // Framebuffer ID
    GLuint framebuffer_id = 0;
    // Color texture ID
    GLuint color_texture_id = 0;
    // Depth renderbuffer ID
    GLuint depth_renderbuffer_id = 0;
    
    // Width & Height of the viewport framebuffer
    int width = 0;
    int height = 0;

    /**
     * @brief Destroy the viewport framebuffer
     * 
     * @return void
     */
    void destroy() {
        // Destroy the depth renderbuffer of the viewport framebuffer
        if (this->depth_renderbuffer_id != 0) {
            glDeleteRenderbuffers(1, &this->depth_renderbuffer_id);
            this->depth_renderbuffer_id = 0;
        }

        // Destroy the color texture of the viewport framebuffer
        if (this->color_texture_id != 0) {
            glDeleteTextures(1, &this->color_texture_id);
            this->color_texture_id = 0;
        }

        // Destroy the framebuffer of the viewport framebuffer
        if (this->framebuffer_id != 0) {
            glDeleteFramebuffers(1, &this->framebuffer_id);
            this->framebuffer_id = 0;
        }

        // Set the width and height of the viewport framebuffer to 0
        this->width = 0;
        this->height = 0;
    }

    /**
     * @brief Create the color texture of the viewport framebuffer
     * 
     * @param texture_width The width of the color texture
     * @param texture_height The height of the color texture
     * @return void
     */
    void create_color_texture(const int texture_width, const int texture_height) {
        // Generate the color texture of the viewport framebuffer
        glGenTextures(1, &this->color_texture_id);
        glBindTexture(GL_TEXTURE_2D, this->color_texture_id);
        
        // Create the image of the color texture
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
        
        // Create the parameters of the color texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /**
     * @brief Create the depth renderbuffer of the viewport framebuffer
     * 
     * @param texture_width The width of the depth renderbuffer
     * @param texture_height The height of the depth renderbuffer
     * @return void
     */
    void create_depth_renderbuffer(const int texture_width, const int texture_height) {
        // Generate the depth renderbuffer of the viewport framebuffer
        glGenRenderbuffers(1, &this->depth_renderbuffer_id);
        glBindRenderbuffer(GL_RENDERBUFFER, this->depth_renderbuffer_id);
        
        // Set the image of the depth renderbuffer
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            texture_width,
            texture_height
        );
        
        // Set the parameters of the depth renderbuffer
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

public:
    ViewportFramebuffer() = default;

    /**
     * @brief Destructor of the viewport framebuffer
     * 
     * @return void
     */
    ~ViewportFramebuffer() {
        this->destroy();
    }

    ViewportFramebuffer(const ViewportFramebuffer&) = delete;
    ViewportFramebuffer& operator=(const ViewportFramebuffer&) = delete;

    /**
     * @brief Resize the viewport framebuffer
     * 
     * @param new_width The new width
     * @param new_height The new height
     * @return void
     */
    void resize(const int new_width, const int new_height) {
        const int clamped_width = std::max(1, new_width);
        const int clamped_height = std::max(1, new_height);

        if (clamped_width == this->width && clamped_height == this->height) {
            return;
        }

        // Destroy the viewport framebuffer
        this->destroy();

        // Set the width and height of the viewport framebuffer
        this->width = clamped_width;
        this->height = clamped_height;

        // Create the color texture of the viewport framebuffer
        this->create_color_texture(this->width, this->height);
        // Create the depth renderbuffer of the viewport framebuffer
        this->create_depth_renderbuffer(this->width, this->height);

        // Generate the framebuffer of the viewport framebuffer
        glGenFramebuffers(1, &this->framebuffer_id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer_id);
        // Attach the color texture to the framebuffer
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            this->color_texture_id,
            0
        );
        // Attach the depth renderbuffer to the framebuffer
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

    /**
     * @brief Unbind the viewport framebuffer
     * 
     * @param default_width The default width
     * @param default_height The default height
     * @return void
     */
    void unbind(const int default_width, const int default_height) const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, default_width, default_height);
    }

    /**
     * @brief Get the color texture ID of the viewport framebuffer
     * 
     * @return GLuint The color texture ID of the viewport framebuffer
     */
    [[nodiscard]] GLuint get_color_texture_id() const noexcept {
        return this->color_texture_id;
    }

    /**
     * @brief Get the width of the viewport framebuffer
     * 
     * @return int The width of the viewport framebuffer
     */
    [[nodiscard]] int get_width() const noexcept {
        return this->width;
    }

    /**
     * @brief Get the height of the viewport framebuffer
     * 
     * @return int The height of the viewport framebuffer
     */
    [[nodiscard]] int get_height() const noexcept {
        return this->height;
    }

    /**
     * @brief Check if the viewport framebuffer is valid
     * 
     * @return true If the viewport framebuffer is valid
     * @return false If the viewport framebuffer is not valid
     */
    [[nodiscard]] bool is_valid() const noexcept {
        return this->framebuffer_id != 0 && this->color_texture_id != 0;
    }
};

#endif
