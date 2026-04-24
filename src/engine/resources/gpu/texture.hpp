//
// Created by tomfr on 08/03/2026.
//

#ifndef ENGINE_TEXTURE_HPP
#define ENGINE_TEXTURE_HPP
#include <glad/glad.h>
#include "engine/core/wrapper/TextureBuffer.hpp"

class Texture
{
    TextureBuffer texture_buffer;

    // Texture parameter
    GLenum texture_target;
    GLint texture_wrap_s;
    GLint texture_wrap_t;
    GLint texture_min_filter;
    GLint texture_mag_filter;

    void initialize_texture(const unsigned char* data, const int width, const int height, const int nrChannels)
    {
        // Bind the texture
        this->texture_buffer.bind();

        // Set the texture data
        this->texture_buffer.set_data(
            this->texture_target, 
            width, 
            height, 
            nrChannels == 4 ? GL_RGBA : GL_RGB, 
            nrChannels == 4 ? GL_RGBA : GL_RGB, 
            GL_UNSIGNED_BYTE, 
            data
        );

        // Set Texture Wrapping
        this->texture_buffer.set_parameter_i(GL_TEXTURE_WRAP_S, this->texture_wrap_s);
        this->texture_buffer.set_parameter_i(GL_TEXTURE_WRAP_T, this->texture_wrap_t);
        // Set texture Mipmaps
        this->texture_buffer.set_parameter_i(GL_TEXTURE_MIN_FILTER, this->texture_min_filter);
        this->texture_buffer.set_parameter_i(GL_TEXTURE_MAG_FILTER, this->texture_mag_filter);

        // Generate Mipmaps
        this->texture_buffer.generate_mipmap();

        // Unbind the texture
        this->texture_buffer.unbind();
    }
public:
    explicit Texture(const unsigned char* data, const int width, const int height, const int nrChannels,  const GLenum texture_target = GL_TEXTURE_2D, const GLint texture_wrap_s = GL_REPEAT, const GLint texture_wrap_t = GL_REPEAT, const GLint texture_min_filter = GL_LINEAR, const GLint texture_mag_filter = GL_LINEAR) : texture_target(texture_target), texture_wrap_s(texture_wrap_s), texture_wrap_t(texture_wrap_t), texture_min_filter(texture_min_filter), texture_mag_filter(texture_mag_filter)
    {
        // Initialize the texture options/ parameters
        this->initialize_texture(
            data,
            width,
            height,
            nrChannels
        );

    }
    ~Texture() = default;
    
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept : texture_buffer(std::move(other.texture_buffer)), texture_target(other.texture_target), texture_wrap_s(other.texture_wrap_s), texture_wrap_t(other.texture_wrap_t), texture_min_filter(other.texture_min_filter), texture_mag_filter(other.texture_mag_filter)
    {
        other.texture_target = 0;
        other.texture_wrap_s = 0;
        other.texture_wrap_t = 0;
        other.texture_min_filter = 0;
        other.texture_mag_filter = 0;
    }

    void bind(const GLenum texture_slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + texture_slot);
        this->texture_buffer.bind();
    }
    void unbind() const
    {
        this->texture_buffer.unbind();
    }
};


#endif //ENGINE_TEXTURE_HPP