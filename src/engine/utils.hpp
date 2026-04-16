//
// Created by tomfr on 03/03/2026.
//
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <glad/glad.h>
#include <vector>

inline uint32_t hash_string(const std::string& str)
{
    return std::hash<std::string>{}(str);
}

template<typename T>
unsigned int makeVBO(const std::vector<T>& data, unsigned int vbo_types, GLenum usage)
{
    unsigned int vbo;

    // Create the Vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(vbo_types, vbo);

    // Copy the data into the buffer
    glBufferData(vbo_types, data.size() * sizeof(T), data.data(), usage);
    glBindBuffer(vbo_types, 0);

    return vbo;
}

inline unsigned int makeShaderProgram(const char* vertex_source, const char* fragment_source)
{
    int succes;
    char infoLog[512];

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(GL_VERTEX_SHADER, GL_COMPILE_STATUS, &succes);
    if (!succes)
    {
        glGetShaderInfoLog(GL_VERTEX_SHADER, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(GL_FRAGMENT_SHADER, GL_COMPILE_STATUS, &succes);
    if (!succes)
    {
        glGetShaderInfoLog(GL_VERTEX_SHADER, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &succes);
    if (!succes)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    return program;
}


#endif



