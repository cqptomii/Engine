/**
 * @file blockview.hpp
 * @author Tom FRAISSE
 * @brief Block view class used as a interface to access the information about a block in the document
 * @version 0.1
 * @date 2026-07-31
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef BLOCKVIEW_HPP
#define BLOCKVIEW_HPP

#include "engine/serialization/document/document.hpp"
#include "engine/serialization/parser/parser_utils.hpp"
#include "engine/serialization/tokenizer/token.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

class BlockView {
private:

    // The document associated with the block view
    const Document* doc_{nullptr};

    // The index of the block in the document
    std::uint32_t block_index_{0};

public:
    /**
     * @brief Default constructor used to initialize the block view
     * 
     */
    BlockView() = default;

    /**
     * @brief Constructor used to initialize the block view with a document and a block index
     * 
     * @param doc : The document associated with the block view
     * @param block_index : The index of the block in the document
     */
    BlockView(const Document* doc, std::uint32_t block_index) noexcept : doc_(doc), block_index_(block_index) {}

    /**
     * @brief Method used to check if the block view is valid
     * 
     * @return true : If the block view is valid
     * @return false : If the block view is not valid
     */
    bool valid() const noexcept{
        return doc_ != nullptr && block_index_ < doc_->blocks().size();
    }


    /**
     * @brief Method used to get the name of the block
     * 
     * @return std::string_view : The name of the block
     */
    std::string_view name() const{
        if(!valid()) return std::string_view();
        return doc_->blocks()[block_index_].name;
    }
    /**
     * @brief Method used to get the line number of the block
     * 
     * @return std::uint32_t : The line number of the block
     */
    std::uint32_t line() const{
        // Check if the block is valid
        if(!valid()) return 0;
        return doc_->blocks()[block_index_].line;
    }

    /**
     * @brief  Method used to get the amount of children of the current block
     * 
     * @return std::uint32_t : The amount of children of the current block
     */
    std::uint32_t child_count() const{
        //Check if the block is valid
        if(!valid()) return 0;
        return doc_->blocks()[block_index_].child_count;
    }

    /**
     * @brief Method used to get the child of the current block at the given index
     * 
     * @param index : The index of the child to get
     * @return BlockView : The child of the current block at the given index
     */
    BlockView child(std::uint32_t index) const{
        // Check if the block is valid and if the index is within the range of children
        if(!valid() || index >= child_count()) 
            return BlockView();

        // Get the block object from the document
        const Block& block = doc_->blocks_[block_index_];

        return BlockView(doc_, block.first_child + index);
    }

    /**
     * @brief Method used to find a child of the current block with the given name
     * 
     * @param name : The name of the child to find
     * @return std::optional<BlockView> : The child of the current block with the given name
     */
    std::optional<BlockView> find_child(std::string_view name) const{

        // Check if the block is valid
        if(!valid()) return std::nullopt;

        // Iterate over the children of the current block
        for (std::uint32_t i = 0; i < child_count(); ++i) {

            // Get the child view
            BlockView child_view = child(i);

            // Check if the child view is valid and if the name of the child is the same as the given name
            if (child_view.name() == name) {
                return child_view;
            }
        }

        return std::nullopt;
    }


    /**
     * @brief Method used to get the amount of properties of the current block
     * 
     * @return std::uint32_t : The amount of properties of the current block
     */
    std::uint32_t property_count() const{
        // Check if the block is valid
        if(!valid()) return 0;

        // return the amount of properties of the current block
        return doc_->blocks_[block_index_].property_count;
    }

    /**
     * @brief Method used to get the values of the properties of the current block with the given name
     * 
     * @param name : The name of the properties to get
     * @return std::span<const Token> : The values of the properties of the current block with the given name
     */
    std::span<const Token> property_values(std::string_view name) const{
        // Check if the block is valid
        if(!valid()) return std::span<const Token>();

        const Block& block = doc_->blocks_[block_index_];

        // Iterate over the properties of the current block
        for (std::uint32_t i = 0; i < property_count(); ++i) {
            
            // Get the property object from the doc
            const Property& property = doc_->properties_[block.first_property + i];    

            // Check if the property name is the same as the given name
            if (property.key == name){
                
                // Get the first value of the property and the amount of values
                std::uint32_t first_value = property.first_value;
                std::uint32_t amount_values = property.value_count;

                // Create the span of values
                return std::span<const Token>(
                    doc_->values_.data() + first_value,
                     amount_values
                );

            }

        }

        return std::span<const Token>();

    }


    //
    // Read METHODS
    //

    /**
     * @brief Method used to read a float value from the properties of the current block with the given name
     * 
     * @param key : The name of the property to read
     * @param out : The output float value
     * @return true : If the reading was successful
     * @return false : If the reading was not successful
     */
    bool read(std::string_view key, float& out){
        // Get the values of the property with the given name
        const std::span<const Token> values = property_values(key);

        // Check if the values are valid and if the value is a float
        if (values.size() != 1 || values[0].token_type != TokenType::Number) {
            return false;
        }

        // Parse the float value
        return parse_float(values[0].token_value, out);
    }

    /**
     * @brief Method used to read a double value from the properties of the current block with the given name
     * 
     * @param key : The name of the property to read
     * @param out : The output double value
     * @return true : If the reading was successful
     * @return false : If the reading was not successful
     */
    bool read(std::string_view key, std::uint64_t& out){
        // Get the values of the property with the given name
        const std::span<const Token> values = property_values(key);
        
        // Check if the values are valid and if the value is a uint64_t
        if (values.size() != 1 || values[0].token_type != TokenType::Number) {
            return false;
        }

        return parse_uint64_t(values[0].token_value, out);
    }

    /**
     * @brief Method used to read a vec3 value from the properties of the current block with the given name
     * 
     * @param key : The name of the property to read
     * @param out : The output vec3 value
     * @return true : If the reading was successful
     * @return false : If the reading was not successful
     */
    bool read(std::string_view key, glm::vec3& out){
        const std::span<const Token> values = property_values(key);
        // Check if there are at least 3 values for a vec3
        if (values.size() < 3) {
            return false;
        }

        // Check if the values are valid numbers
        for(std::size_t i = 0; i < 3; i++){
            // Check the token type
            if(values[i].token_type != TokenType::Number){
                return false;
            }

            if (!parse_float(values[i].token_value, out[i])){
                return false;
            }
        }

        return true;
    }
    
    /**
     * @brief Method used to read a quat value from the properties of the current block with the given name
     * 
     * @param key : The name of the property to read
     * @param out : The output quat value
     * @return true : If the reading was successful
     * @return false : If the reading was not successful
     */
    bool read(std::string_view key, glm::quat& out){
        // Get the values of the property with the given key identifier
        const std::span<const Token> values = property_values(key);

        // Check if there are at least 4 values for the quat
        if(values.size() != 4) {
            return false;
        }


        float components[4];
        // Check if each value is a valid number
        for (std::size_t i = 0; i < 4; i++){
            // Check the token type
            if(values[i].token_type != TokenType::Number){
                return false;
            }

            if (!parse_float(values[i].token_value, components[i])){
                return false;
            }
        }

        // Fill the quat with each valid values 
        out = glm::quat(components[0], components[1], components[2], components[3]);

        return true;
    }
    
    /**
     * @brief Method used to read a string value from the properties of the current block with the given name
     * 
     * @param key : The name of the property to read
     * @param out : The output string value
     * @return true : If the reading was successful
     * @return false : If the reading was not successful
     */
    bool read(std::string_view key, std::string& out)const {
        const std::span<const Token> values = property_values(key);
        if (values.size() != 1 || values[0].token_type != TokenType::String) {
            return false;
        }
        return decode_escaped_string(values[0].token_value, out);
    }

};

#endif