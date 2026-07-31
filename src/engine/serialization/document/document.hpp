/**
 * @file document.hpp
 * @author Tom FRAISSE
 * @brief Document structures for the serialization engine
 * @version 0.1
 * @date 2026-07-31
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include "engine/serialization/tokenizer/token.hpp"
#include "engine/serialization/parser/parser.hpp"
#include <optional>
#include <string_view>
#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <optional>

class BlockView;

/**
 * @brief Document property structure used to store the properties of a block
 * 
 * @param key The property key
 * @param first_value The index of the first value in the values array
 * @param value_count The number of values in the property
 */
struct Property{
    std::string_view key;
    std::uint32_t first_value;
    std::uint32_t value_count;
};

/**
 * @brief Block structure used to store all the information about each block in the document
 * 
 * @param name The name of the block
 * @param line The line number of the block in the source code
 * @param first_child The index of the first child block
 * @param child_count The number of children blocks
 * @param first_property The index of the first property
 * @param property_count The number of properties
 */
struct Block{
    std::string_view name;
    std::uint32_t line;
    std::uint32_t first_child;
    std::uint32_t child_count;
    std::uint32_t first_property;
    std::uint32_t property_count;
};


/**
 * @brief Document class used to store the document and all the information about it
 * 
 */
class Document{
private:
    
    // The source string of the document
    std::string source_;

    // List of blocks found in the document 
    friend class BlockView;
    std::vector<Block> blocks_;
    
    // List of properties found in the document
    std::vector<Property> properties_;

    // List of values found in the document
    std::vector<Token> values_; 

    /**
     * @brief Private constructor used to initialize the document from a source string
     * @details the constructor is friend with the Parser class to allow it to call the private constructor
     * @param source : The source string of the document
     */
    friend class Parser;
    explicit Document(std::string_view source) noexcept : source_(std::move(source)) {}

public:
    /**
     * @brief Method used to parse the document from a source string
     * @details The method is used to parse the document from a source string and initialize the document
     * @param source : The source string of the document
     * @return Document : The document object
     */
    static Document parse(std::string_view source) noexcept;

    /**
     * @brief Method used to check if the document is empty
     * 
     * @return true : The document is empty
     * @return false : The document is not empty
     */
    [[nodiscard]] bool empty() const noexcept{
        return blocks_.empty();
    }

    /**
     * @brief Method used to get the root block of the document
     * 
     * @return BlockView : The root block of the document
     */
    [[nodiscard]] std::optional<BlockView> root() const;

    /*
     * @brief Method used to get the list of blocks in the document
     * 
     * @return std::span<const Block> : The list of blocks in the document
     */
    std::span<const Block> blocks() const{
        return std::span<const Block>(blocks_.data(), blocks_.size());
    }
};



#endif