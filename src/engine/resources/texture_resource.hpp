//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_TEXTURE_RESOURCE_HPP
#define ENGINE_TEXTURE_RESOURCE_HPP
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <ostream>
#include "../src/stb_image.h"

class TextureResource
{

    int width, height, nrChannels;
    unsigned char *data;

public:
    explicit TextureResource(const char* path)
    {
        this->load_texture(path);
    }
    ~TextureResource()
    {
        delete_texture();
    }

    void load_texture(const char* path)
    {
        stbi_set_flip_vertically_on_load(true);
        this->data = stbi_load(path, &this->width, &this->height, &this->nrChannels, 0);
        if (!this->data)
        {
            std::cout << "ERROR::Failed to load Texture::Texture data is null" << std::endl;
        }
    }
    void delete_texture() const
    {
        stbi_image_free(this->data);
    }

    // Getters Methods
    unsigned char* get_data() const
    {
        return this->data;
    }
    int get_width() const
    {
        return this->width;
    }
    int get_height() const
    {
        return this->height;
    }
    int get_nr_channels() const
    {
        return this->nrChannels;
    }
};

#endif //ENGINE_TEXTURE_RESOURCE_HPP