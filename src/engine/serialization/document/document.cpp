#include "engine/serialization/document/document.hpp"
#include "engine/serialization/document/blockview.hpp"
#include "engine/serialization/tokenizer/tokenizer.hpp"
#include "engine/serialization/tokenizer/token.hpp"
#include "engine/serialization/parser/parser.hpp"

/**
 * @brief Method used to get the root block of the document
 * 
 * @return std::optional<BlockView> : the root block of the document
 */
 std::optional<BlockView> Document::root() const{
    //Check if the document is empty
    if(this->blocks_.empty())
        return std::nullopt;

    //Return the root block
    return BlockView(this, 0);
}

Document Document::parse(std::string source) {

    // Create the document object
    Document document(std::move(source));

    // Create the tokenizer object
    Tokenizer tokenizer(document.source_);

    std::vector<Token> tokens;
    
    // Iterate over the tokens until the end of the file
    for (;;) {
        Token t = tokenizer.next();
        tokens.push_back(t);

        if (t.token_type == TokenType::EndOfFile) break;
    }

    // Create the parser object
    Parser parser(document);

    // Move the tokens to the parser
    parser.set_tokens(std::move(tokens)); // ou constructeur Parser(doc, tokens)

    // Parse tokens to blocks and properties
    parser.parse_document();

    // Return the created document
    return document;
}