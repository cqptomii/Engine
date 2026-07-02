#ifndef TEXTURE_BUFFER_HPP
#define TEXTURE_BUFFER_HPP

#include <glad/glad.h>
class TextureBuffer
{
private:
    GLuint texture_id;
public:
    TextureBuffer() : texture_id(0) {
        glGenTextures(1, &texture_id);
    }
    ~TextureBuffer() {
        if (texture_id != 0) {
            glDeleteTextures(1, &texture_id);
        }
    }

    TextureBuffer(const TextureBuffer&) = delete;
    TextureBuffer& operator=(const TextureBuffer&) = delete;
    TextureBuffer(TextureBuffer&& other) noexcept : texture_id(other.texture_id) {
        other.texture_id = 0;
    }

    // Texture data setup
    void set_data(GLenum texture_target, GLsizei width, GLsizei height, GLenum internalFormat, GLenum format, GLenum type, const void* data) {
        if(texture_target == GL_TEXTURE_1D){
            glTexImage1D(texture_target, 0, internalFormat, width, 0, format, type, data);
            return;
        }else if(texture_target == GL_TEXTURE_2D){
            glTexImage2D(texture_target, 0, internalFormat, width, height, 0, format, type, data);
            return;
        }else if(texture_target == GL_TEXTURE_3D){
            // Texture 3D
        }
    }

    // Texture parameter setting
    void set_parameter_i(GLenum pname, GLint param) {
        glTexParameteri(GL_TEXTURE_BUFFER, pname, param);
    }
    void set_parameter_f(GLenum pname, GLfloat param) {
        glTexParameterf(GL_TEXTURE_BUFFER, pname, param);
    }
    void set_parameter_iv(GLenum pname, const GLint* params) {
        glTexParameteriv(GL_TEXTURE_BUFFER, pname, params);
    }
    void set_parameter_fv(GLenum pname, const GLfloat* params) {
        glTexParameterfv(GL_TEXTURE_BUFFER, pname, params);
    }
    void set_parameter_iu(GLenum pname, GLuint param) {
        glTexParameterIuiv(GL_TEXTURE_BUFFER, pname, &param);
    }

    // Mipmap generation
    void generate_mipmap() {
        glGenerateMipmap(GL_TEXTURE_BUFFER);
    }


    // Binding and unbinding
    void bind() const {
        glBindTexture(GL_TEXTURE_BUFFER, texture_id);
    }
    void unbind() const {
        glBindTexture(GL_TEXTURE_BUFFER, 0);
    }
};

#endif // TEXTURE_BUFFER_HPP