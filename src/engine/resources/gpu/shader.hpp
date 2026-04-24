//
// Created by tomfr on 08/03/2026.
//

#ifndef ENGINE_SHADER_HPP
#define ENGINE_SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <string>
#include <iostream>

class Shader
{
    unsigned int program_id;

    mutable std::unordered_map<std::string, GLint> uniform_locations;
public:
    explicit Shader(const char* vertex_shader_str, const char* fragment_shader_str) : program_id(0)
    {
        int success;
        char infoLog[512];

        // Compile the vertex shader
        unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_str, nullptr);
        glCompileShader(vertex_shader);

        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Compile the Fragment shader
        unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_str, nullptr);
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Shader program creation
        this->program_id = glCreateProgram();
        glAttachShader(this->program_id, vertex_shader);
        glAttachShader(this->program_id, fragment_shader);
        glLinkProgram(this->program_id);

        glGetProgramiv(this->program_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->program_id, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
    ~Shader()
    {
        if (this->program_id != 0)
        {
            glDeleteProgram(this->program_id);
        }
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept : program_id(other.program_id), uniform_locations(std::move(other.uniform_locations))
    {
        other.program_id = 0;
    }

    // Use the shader
    void use() const
    {
        glUseProgram(this->program_id);
    }
    static void unuse()
    {
        glUseProgram(0);
    }

    // Uniform caching and retrieval
    GLint get_location(const std::string& name) const
    {
        auto it = uniform_locations.find(name);
        if (it != uniform_locations.end())
            return it->second;

        GLint loc = glGetUniformLocation(program_id, name.c_str());
        uniform_locations[name] = loc;
        return loc;
    }

    // UBO Binding
    void bind_ubo(const std::string& block_name, GLuint binding_point) const
    {
        GLuint block_index = glGetUniformBlockIndex(program_id, block_name.c_str());
        if (block_index != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(program_id, block_index, binding_point);
        }
        else
        {
            std::cout << "WARNING::SHADER::UBO_BLOCK_NOT_FOUND: " << block_name << std::endl;
        }
    }


    // Bind Uniform value into the shader
    void set_bool(const std::string& name, bool value) const
    {
        glUniform1i(get_location(name), (int)value);
    }
    void set_int(const std::string& name, int value) const
    {
        glUniform1i(get_location(name), value);
    }
    void set_float(const std::string& name, float value) const
    {
        glUniform1f(get_location(name), value);
    }

    void set_vec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(get_location(name), 1, glm::value_ptr(value));
    }
    void set_vec2(const std::string& name, float x, float y) const
    {
        glUniform2f(get_location(name), x, y);
    }
    void set_vec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(get_location(name), 1, glm::value_ptr(value));
    }
    void set_vec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(get_location(name), x, y, z);
    }
    void set_vec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(get_location(name), 1, glm::value_ptr(value));
    }
    void set_vec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(get_location(name), x, y, z, w);
    }

    void set_mat2(const std::string& name, const glm::mat2& matrix) const
    {
        glUniformMatrix2fv(get_location(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void set_mat3(const std::string& name, const glm::mat3& matrix) const
    {
        glUniformMatrix3fv(get_location(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void set_mat4(const std::string& name, const glm::mat4& matrix) const
    {
        glUniformMatrix4fv(get_location(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
};


#endif //ENGINE_SHADER_HPP