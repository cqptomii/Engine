//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_SHADER_RESOURCE_HPP
#define ENGINE_SHADER_RESOURCE_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderResource
{
    std::string vertex_source;
    std::string fragment_source;
public:
    ShaderResource(const char* vertex_path, const char* fragment_path)
    {
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
            this->fragment_source = fragment_stream.str();
            this->vertex_source = vertex_stream.str();
        } catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
    }

    const std::string& get_vertex_source() const { return this->vertex_source; }
    const std::string& get_fragment_source() const { return this->fragment_source; }
};


#endif //ENGINE_SHADER_RESOURCE_HPP