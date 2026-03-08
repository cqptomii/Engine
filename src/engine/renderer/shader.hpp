//
// Created by tomfr on 08/03/2026.
//

#ifndef ENGINE_SHADER_HPP
#define ENGINE_SHADER_HPP

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader
{
    unsigned int program_id;
public:

    Shader(const char* vertex_path, const char* fragment_path) : program_id(0)
    {
        std::string vertex_source;
        std::string fragment_source;
        std::ifstream vertex_shader_file;
        std::ifstream fragment_shader_file;

        vertex_shader_file.exceptions( std::ifstream::failbit | std::ifstream::badbit );
        fragment_shader_file.exceptions( std::ifstream::failbit | std::ifstream::badbit );
        try
        {
            vertex_shader_file.open(vertex_path);
            fragment_shader_file.open(fragment_path);
            std::stringstream vertex_stream, fragment_stream;

            // read file
            vertex_stream << vertex_shader_file.rdbuf();
            fragment_stream << fragment_shader_file.rdbuf();

            // Close each file
            vertex_shader_file.close();
            fragment_shader_file.close();

            // Get the string associated which each shader
            fragment_source = fragment_stream.str();
            vertex_source = vertex_stream.str();
        } catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }

        const char* vertex_shader_str = vertex_source.c_str();
        const char* fragment_shader_str = fragment_source.c_str();

        unsigned int vertex_shader, fragment_shader;
        int success;
        char infoLog[512];

        // Compile the vertex shader
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_str, nullptr);
        glCompileShader(vertex_shader);

        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Compile the Fragment shader
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
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

    // Use the shader
    void bind() const
    {
        glUseProgram(this->program_id);
    }

    // Bind Uniform value into the shader
    void set_bool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(this->program_id, name.c_str()), (int)value);
    }
    void set_int(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(this->program_id, name.c_str()), value);
    }
    void set_float(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(this->program_id, name.c_str()), value);
    }

    void set_vec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(this->program_id, name.c_str()), 1, glm::value_ptr(value));
    }
    void set_vec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(this->program_id, name.c_str()), x, y);
    }

    void set_vec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(this->program_id, name.c_str()), 1, glm::value_ptr(value));
    }
    void set_vec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(this->program_id, name.c_str()), x, y, z);
    }

    void set_vec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(this->program_id, name.c_str()), 1, glm::value_ptr(value));
    }
    void set_vec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(this->program_id, name.c_str()), x, y, z, w);
    }

    void set_mat2(const std::string& name, const glm::mat2& matrix) const
    {
        glUniformMatrix2fv(glGetUniformLocation(this->program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void set_mat3(const std::string& name, const glm::mat3& matrix) const
    {
        glUniformMatrix3fv(glGetUniformLocation(this->program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void set_mat4(const std::string& name, const glm::mat4& matrix) const
    {
        glUniformMatrix4fv(glGetUniformLocation(this->program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }

};


#endif //ENGINE_SHADER_HPP