//
// Created by tomfr on 03/03/2026.
//
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <glad/glad.h>
#include <vector>
#include <filesystem>
#include <string>
#include <array>
#include <algorithm>
#include <cctype>

inline uint32_t hash_string(const std::string& str)
{
    return std::hash<std::string>{}(str);
}
inline std::string normalize_resource_path_minimal(const std::string& path)
{
    if (path.empty())
    {
        return path;
    }

    std::filesystem::path fs_path(path);
    std::string normalized = fs_path.lexically_normal().generic_string();

#ifdef _WIN32
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](const unsigned char c)
        {
            return static_cast<char>(std::tolower(c));
        });
#endif

    while (normalized.size() > 1 && normalized.back() == '/')
    {
        normalized.pop_back();
    }

    return normalized;
}
inline std::string resolve_shader_path(const std::string& relative_path)
{
    if (std::filesystem::exists(relative_path))
    {
        return relative_path;
    }

    const std::array<std::string, 4> candidates = {
        "../" + relative_path,
        "../../" + relative_path,
        "../../../" + relative_path,
        "../../../../" + relative_path
    };

    for (const auto& candidate : candidates)
    {
        if (std::filesystem::exists(candidate))
        {
            return candidate;
        }
    }

    return relative_path;
}
inline void convert_hex_to_rgba(const unsigned int hex_color, float& r, float& g, float& b, float& a)
{
    r = static_cast<float>((hex_color >> 16) & 0xFF) / 255.0f;
    g = static_cast<float>((hex_color >> 8) & 0xFF) / 255.0f;
    b = static_cast<float>(hex_color & 0xFF) / 255.0f;
    a = 1.0f;
}


#endif // UTILS_H



