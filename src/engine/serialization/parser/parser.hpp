/**
 * @file parser.hpp
 * @author Tom FRAISSE
 * @brief Parser class used to parse the document and extract the information about the blocks and properties
 * @version 0.1
 * @date 2026-07-31
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef PARSER_HPP
#define PARSER_HPP

#include "engine/serialization/document/document.hpp"
#include "engine/serialization/tokenizer/token.hpp"
#include <cstddef>
#include <vector>
#include <stdexcept>

class Parser {

private:

    // Reference to the document parsed
    Document& document_;

    // list of tokens parsed
    std::vector<Token> tokens_;

    // Current index in the tokens list
    std::size_t index_{0};

public:

    /**
     * @brief Default constructor used to initialize the parser with the document to parse
     * 
     * @param document : The document to parse
     */
    Parser(Document& document) : document_(document) {}

    /**
     * @brief Constructor used to initialize the parser with the document and the tokens to parse
     * 
     * @param document : The document to parse
     * @param tokens : The tokens to parse
     */
    Parser(Document& document, std::vector<Token> tokens) : document_(document), tokens_(std::move(tokens)) {}

    /**
     * @brief Default destructor of the class
     * 
     */
    ~Parser() = default;

    // Delete unusable constructors
    Parser() = delete;
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;


    //
    // Parsing methods
    //

    /**
     * @brief Method used to parse the whole document
     * @details The document is a collection of blocks and properties
     * @example "ENTITY{[Property1], [Property2], [Block1], [Block2]}"
     * 
     */
    void parse_document(){

        while(peek().token_type != TokenType::EndOfFile){
            parse_block();
        }

    }


    /**
     * @brief Method used to parse a block of the document delimited by brackets
     * @details A block is a collection of properties and blocks
     * @example " ENTITY{[Property1], [Property2], [Block1], [Block2]}"
     */
    void parse_block(){

        const Token& block_name = advance();

        // Check if there is a block identifier 
        if(block_name.token_type != TokenType::Identifier){
            throw std::runtime_error("Invalid block identifier");
        }

        // Check if the next token is an LeftBrace TokenType
        if(!expect(TokenType::LeftBrace)){
            throw std::runtime_error("Invalid left brace token for block");
        }

    
        // Save the current block information
        const std::uint32_t block_line = block_name.line;
        const std::uint32_t block_index = static_cast<std::uint32_t>(document_.blocks_.size());
        const std::uint32_t first_property = static_cast<std::uint32_t>(document_.properties_.size());
        const std::uint32_t first_child = static_cast<std::uint32_t>(document_.blocks_.size() + 1);
        
        // Add the block into the document
        document_.blocks_.emplace_back(Block{
            block_name.token_value,
            block_line,
            0,
            0,
            first_property,
            0
        });

        // Parse the content of the block

        while(peek().token_type != TokenType::RightBrace){
            // If the next token is an identifier and the next token is an LeftBrace then parse a block
            if( peek().token_type == TokenType::Identifier && peek(1).token_type == TokenType::LeftBrace){
                parse_block();
            }
            // else if the next token is an identifier parse a property
            else if (peek().token_type == TokenType::Identifier){
                parse_property();
            }
            // else throw an error
            else{
                throw std::runtime_error("Invalid token for block content");
            }
        }

        // Reed RightBrace
        if(!expect(TokenType::RightBrace)){
            throw std::runtime_error("Invalid right brace token for block");
        }

        // Update the block information 
        Block& block = document_.blocks_[block_index];
        block.first_child = first_child;
        block.child_count = static_cast<std::uint32_t>(document_.blocks_.size()) - first_child;
        block.property_count = static_cast<std::uint32_t>(document_.properties_.size()) - first_property;
        block.line = block_line;
    }
    
    /**
     * @brief Method used to parse a line of the document that is a property
     * @details A property is a key-value pair separated by an EQUALS TokenType
     * @example "'Id' : 123"
     * @throw std::runtime_error : If the key is not a valid identifier
     * @throw std::runtime_error : If the equals token is not found
     */
    void parse_property(){

        // Get the current token
        const Token& key = advance();

        // Check if the key is a valid identifier
        if(key.token_type != TokenType::Identifier){
            throw std::runtime_error("Invalid key for property");
        }

        // Check if the next token is a EQUALS token

        if(!expect(TokenType::EQUALS)){
            throw std::runtime_error("Invalid equals token for property");
        }

        // Identify the first value token
        const std::uint32_t first_value = static_cast<std::uint32_t>(document_.values_.size());

        while(is_value_token(peek())){
            // Get the value token
            const Token& value = advance();

            // Add the value to the document
            document_.values_.push_back(std::move(value));
        }

        // Count the number of values
        const std::uint32_t value_count = static_cast<std::uint32_t>(document_.values_.size() - first_value);

        // Add the property into the document
        document_.properties_.emplace_back(Property{key.token_value, first_value, value_count});
    }

    //
    // Helpers methods
    //

    /**
     * @brief Method used to get the token at the current index + the offset
     * 
     * @param offset : The offset to the current index
     * @return const Token& : The token at the current index + the offset
     */
    const Token& peek(std::size_t offset = 0) const{
        // Get the index to peek
        std::size_t peek_index = index_ + offset;

        // Check if the index is out of bounds
        if (peek_index >= tokens_.size()) return tokens_.back();

        // Return the token at the index
        return tokens_[peek_index];
    }

    /**
     * @brief Method used to advance the current index to the next token
     * 
     * @return const Token& : The token at the current index
     */
    const Token& advance(){
        if (index_ >= tokens_.size()) return tokens_.back();
        
        // Return the token at the current index and increment the index
        return tokens_[index_++];
    }

    /**
     * @brief Method used to check if the current token is of the given TokenType
     * 
     * @param kind : The TokenType to check
     * @return true : If the current token is of the given TokenType
     * @return false : If the current token is not of the given TokenType
     */
    bool expect(TokenType kind){
        // Check if the current token is of the given TokenType
        if(peek().token_type == kind){
            // Advance the current index
            advance();
            return true;
        }

        return false;
    }

    /**
     * @brief Method used to check if the given token is a value token
     * @details A value is a token within TokenType Number, String or Identifier
     * @param token : The token to check
     * @return true : If the token is a value token
     * @return false : If the token is not a value token
     */
    bool is_value_token(const Token& token) const{
        switch (token.token_type) {
            case TokenType::Number:
            case TokenType::String:
            case TokenType::Identifier:
                return true;
            default:
                return false;
        }
    }
   

    bool set_tokens(std::vector<Token> tokens){
        // Check if the tokens are not empty
        if (tokens.empty()) {
            return false;
        }

        // Move the tokens to the parser
        tokens_ = std::move(tokens);
        index_ = 0;
        return true;
    }
};

#endif