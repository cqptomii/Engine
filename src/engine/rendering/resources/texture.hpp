//
// Created by tomfr on 08/03/2026.
//

#ifndef ENGINE_TEXTURE_HPP
#define ENGINE_TEXTURE_HPP
#include <glad/glad.h>
#include "../resources/texture_resource.hpp"


class Texture
{
    unsigned int texture_id{};

    // Texture parameter
    GLenum texture_target;
    GLint texture_wrap_s;
    GLint texture_wrap_t;
    GLint texture_min_filter;
    GLint texture_mag_filter;

    void initialize_texture(const unsigned char* data, const int width, const int height, const int nrChannels)
    {
        // Generate the texture
        glGenTextures(1, &this->texture_id);
        glBindTexture(this->texture_target, this->texture_id);

        if (this->texture_target == GL_TEXTURE_1D)
        {
            glTexImage1D(this->texture_target, 0, GL_RGB, width, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }else if (this->texture_target == GL_TEXTURE_2D)
        {
            glTexImage2D(this->texture_target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (this->texture_target == GL_TEXTURE_3D)
        {
            // Texture 3D
        }
        // Set Texture Wrapping
        glTexParameteri(this->texture_target, GL_TEXTURE_WRAP_S, this->texture_wrap_s);
        glTexParameteri(this->texture_target, GL_TEXTURE_WRAP_T, this->texture_wrap_t);

        // Set texture Mipmaps
        glTexParameteri(this->texture_target, GL_TEXTURE_MIN_FILTER, this->texture_min_filter);
        glTexParameteri(this->texture_target, GL_TEXTURE_MAG_FILTER, this->texture_mag_filter);

        // Generate Mipmaps
        glGenerateMipmap(this->texture_target);
    }
public:
    explicit Texture(const TextureResource& resource,  const GLenum texture_target = GL_TEXTURE_2D, const GLint texture_wrap_s = GL_REPEAT, const GLint texture_wrap_t = GL_REPEAT, const GLint texture_min_filter = GL_LINEAR, const GLint texture_mag_filter = GL_LINEAR) : texture_target(texture_target), texture_wrap_s(texture_wrap_s), texture_wrap_t(texture_wrap_t), texture_min_filter(texture_min_filter), texture_mag_filter(texture_mag_filter)
    {
        // Initialize the texture options/ parameters
        this->initialize_texture(
            resource.get_data(),
            resource.get_width(),
            resource.get_height(),
            resource.get_nr_channels()
        );

    }
    ~Texture()
    {
        glDeleteTextures(1, &this->texture_id);
    }

    void bind(const GLenum texture_slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + texture_slot);
        glBindTexture(this->texture_target, this->texture_id);
    }
    void unbind() const
    {
        glBindTexture(this->texture_target, 0);
    }
};


#endif //ENGINE_TEXTURE_HPP