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
#include <vector>

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

    /**
     * @brief Method used to get the token at the current index + the offset
     * 
     * @param offset : The offset to the current index
     * @return const Token& : The token at the current index + the offset
     */
    const Token& peek(std::size_t offset = 0) const;

    /**
     * @brief Method used to advance the current index to the next token
     * 
     * @return const Token& : The token at the current index
     */
    const Token& advance();
   

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