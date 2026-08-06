/**
 * @file parse_utils.hpp
 * @author TOM FRAISSE
 * @brief Utility function for parsing serialized data
 * @version 0.1
 * @date 2026-08-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

#include <string_view>
#include <glm/glm.hpp>
#include <string>

#include <charconv>
#include <cmath>

/**
 * @brief Method used to parse a float value from a string
 * 
 * @param data : The string to parse the float value from  
 * @param out : The output float value
 * @return true : If the parsing was successful
 * @return false : If the parsing was not successful
 */
inline bool parse_float(std::string_view data, float& out){
    float value = 0.0f;

    // Parse the float value from the string 
    const std::from_chars_result result = std::from_chars(
        data.data(), 
        data.data() + data.size(), 
        value
    );

    // Check if the parsing was successful
    if(result.ec != std::errc{} || result.ptr != data.data() + data.size()) {
        return false;
    }

    out = value;
    return true;
}

/**
 * @brief Method used to parse a double value from a string
 * 
 * @param data : The string to parse the double value from
 * @param out : The output double value
 * @return true : If the parsing was successful
 * @return false : If the parsing was not successful
 */
inline bool parse_double(std::string_view data, double& out){
    double value = 0.0;

    // Parse the double value from the string
    const std::from_chars_result result = std::from_chars(
        data.data(), 
        data.data() + data.size(),
        value
    );

    // Check if the parsing was successful
    if( result.ec == std::errc{} || result.ptr != data.data() + data.size()) {
        return false;
    } 

    out = value;
    return true;
}

/**
 * @brief Method used to parse a uint64_t value from a string
 * 
 * @param data : The string to parse the uint64_t value from
 * @param out : The output uint64_t value
 * @return true : If the parsing was successful
 * @return false : If the parsing was not successful
 */
inline bool parse_uint64_t(std::string_view data, std::uint64_t& out){
    std::uint64_t value = 0;

    // Parse the uint64_t value from the string
    const std::from_chars_result result = std::from_chars(
        data.data(),
        data.data() + data.size(),
        value
    );

    // Check if the parsing was successful
    if( result.ec == std::errc{} || result.ptr != data.data() + data.size()){
        return false;
    }

    out = value;
    return true;
}

/**
 * @brief Method used to decode an escaped string
 * @details The method is used to decode an escaped string and remove the escape characters
 * 
 * @param raw : The raw string to decode
 * @param out : The output decoded string
 * @return true : If the decoding was successful
 * @return false : If the decoding was not successful
 */
inline bool decode_escaped_string(std::string_view raw, std::string& out) {
    // Clear & allocate the output string
    out.clear();
    out.reserve(raw.size());
    
    // Iterate over the raw string
    for (std::size_t i = 0; i < raw.size(); ++i) {
        
        // Check if the current character is an escape character
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            // Add the next character to the output string
            out.push_back(raw[++i]);
            continue;
        }
        out.push_back(raw[i]);
    }
    return true;
}