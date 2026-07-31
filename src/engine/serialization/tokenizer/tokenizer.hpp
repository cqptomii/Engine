/**
 * @file tokenizer.hpp
 * @author TOM FRAISSE
 * @brief Tokenizer class for the serialization system
 * @brief 
 * @version 0.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "engine/serialization/tokenizer/token.hpp"
#include <cctype>
#include <string_view>

class Tokenizer{
    private:
        std::string_view m_input;
        std::uint32_t m_current_index{0};
        std::uint32_t m_current_line{1};
        std::uint32_t m_current_column{1};

        /**
         * @brief Method to convert a character to an unsigned char
         * 
         * @param character : The character to convert
         * @return unsigned char : The unsigned char of the character
         */
        static unsigned char to_unsigned_char(const char character) {
            return static_cast<unsigned char>(character);
        }

        /**
         * @brief Method to check if a character is a valid identifier character
         * 
         * @param character : The character to check
         * @return true : If the character is a valid identifier start
         * @return false : If the character is not a valid identifier start
         */
        bool is_identifier_start(const char character) {
            const unsigned char c = to_unsigned_char(character);
            return std::isalpha(c) || c == '_';
        }

        /**
         * @brief Method to check if a character is a valid identifier character
         * 
         * @param character : The character to check
         * @return true : If the character is a valid identifier character
         * @return false : If the character is not a valid identifier character
         */
        bool is_identifier_body(const char character) {
            const unsigned char c = to_unsigned_char(character);
            return std::isalnum(c) || c == '_';
        }

        /**
         * @brief Method to skip whitespace
         * 
         */
        void skip_whitespace() {
            while (!at_end() && std::isspace(to_unsigned_char(m_input[m_current_index]))) {
                if (m_input[m_current_index] == '\n') {
                    ++m_current_index;
                    ++m_current_line;
                    m_current_column = 1;
                } else {
                    ++m_current_index;
                    ++m_current_column;
                }
            }
        }

        /**
         * @brief Method to skip a comment line
         * 
         */
        void skip_comment_line() {
            while (m_current_index < m_input.size() && m_input[m_current_index] != '\n') {
                ++m_current_index;
                ++m_current_column;
            }
        }

    public:
            
        /**
         * @brief Constructor that takes a string_view as input
         * 
         * @param input The input string to tokenize
         */
        explicit Tokenizer(std::string_view input) : m_input(input){}

        /**
         * @brief Default destructor
         * 
         */
        ~Tokenizer() = default; 

        /**
         * @brief Go through the input and return the next token
         * 
         * @return Token The next token from the input
         */
        Token next(){
            // Skip whitespace
            skip_whitespace();

            // If EOF return EndOfFile token
            if(this->at_end()){
                return Token{
                    TokenType::EndOfFile,
                    "",
                    m_current_line,
                    m_current_column
                };
            }

            // Skip comments
            if(m_input[m_current_index] == '#'){
                skip_comment_line();
                return next();
            }

            const std::uint32_t token_line = m_current_line;
            const std::uint32_t token_column = m_current_column;
            const char current = m_input[m_current_index];

            // Structure tokens 
            switch(current){
                default:
                    break;
                // Opening brace TokenType
                case '{':
                    // Increment the current index and column
                    ++m_current_index;
                    ++m_current_column;


                    return Token{
                        TokenType::LeftBrace,
                        "{",
                        token_line,
                        token_column
                    };
                    break;
                // Closing brace TokenType
                case '}':
                    // Increment the current index and column
                    ++m_current_index;
                    ++m_current_column;

                    return Token{
                        TokenType::RightBrace,
                        "}",
                        token_line,
                        token_column
                    };
                    break;
                // Equals TokenType
                case ':':
                    // Increment the current index and column
                    ++m_current_index;
                    ++m_current_column;

                    return Token{
                        TokenType::EQUALS,
                        ":",
                        token_line,
                        token_column
                    };
                
                // String TokenType
                case '"': {
                    // Skip the double quotes
                    ++m_current_index;
                    ++m_current_column;
                    
                    const std::uint32_t start_index = m_current_index;

                    while(true){
                        // Check if we are at the end of the input
                        if(this->at_end()){
                            // Return the token with the temporary buffer as the value and the type INVALID
                            return Token{
                                TokenType::INVALID,
                                m_input.substr(start_index, m_current_index - start_index),
                                token_line,
                                token_column
                            };
                        }

                        if (m_input[m_current_index] == '"') {
                            break;
                        }
    
                        if (m_input[m_current_index] == '\\' && m_current_index + 1 < m_input.size()) {
                            m_current_index += 2;
                            m_current_column += 2;
                            continue;
                        }

                        ++m_current_index;
                        ++m_current_column;

                        
                    }

                    const std::string_view string_value = m_input.substr(start_index, m_current_index - start_index);
                    
                    ++m_current_index;
                    ++m_current_column;

                    return Token{
                        TokenType::String,
                        string_value,
                        token_line,
                        token_column
                    };
                }
            }

            // Identifier TokenType
            if(is_identifier_start(current)){
                const std::uint32_t start_index = m_current_index;

                while (!at_end() && is_identifier_body(m_input[m_current_index])) {
                    ++m_current_index;
                    ++m_current_column;
                }

                // Return the identifier token
                return Token{
                    TokenType::Identifier,
                    m_input.substr(start_index, m_current_index - start_index),
                    token_line,
                    token_column
                };
            }

            // Number TokenType
            if (
                std::isdigit(to_unsigned_char(current)) ||
                current == '.' ||
                current == '-'
            ) {
                const std::uint32_t start_index = m_current_index;
                
                
                // '.' counter to check if we are at the end of the number
                std::uint32_t dot_counter = 0;

                while (!at_end()) {
                    const char character = m_input[m_current_index];

                    if (character == '-') {
                        if (m_current_index != start_index) {
                            break;
                        }
                    }
                
                    if (character == '.') {
                        ++dot_counter;
                        if(dot_counter > 1) {
                            return Token{
                                TokenType::INVALID,
                                m_input.substr(start_index, m_current_index - start_index),
                                token_line,
                                token_column
                            };
                        }
                    }else if (!std::isdigit(to_unsigned_char(character))) {
                        break;
                    }

                    ++m_current_index;
                    ++m_current_column;
                }
                
                // Return the number token
                return Token{
                    TokenType::Number,
                    m_input.substr(start_index, m_current_index - start_index),
                    token_line,
                    token_column
                };
            }

            const std::string_view invalid_value = m_input.substr(m_current_index, 1);
            ++m_current_index;
            ++m_current_column;

            return Token{
                TokenType::INVALID, 
                invalid_value, 
                token_line, 
                token_column
            };
        }


        /**
         * @brief Look at the next token without consuming it
         * 
         * @return Token : The next token from the input
         */
        Token peek() {
        
            // Save the current indexes 
            const std::uint32_t saved_index = m_current_index;
            const std::uint32_t saved_line = m_current_line;
            const std::uint32_t saved_column = m_current_column;
            
            // Get the next token
            Token token = next();
            
            // Restore the current indexes
            m_current_index = saved_index;
            m_current_line = saved_line;
            m_current_column = saved_column;
        
            // Return the token
            return token;
        }

        /**
         * @brief Check if the tokenizer has reached the end of the input
         * 
         * @return true : If the tokenizer has reached the end of the input
         * @return false : If the tokenizer has not reached the end of the input
         */
        bool at_end() const{
            return m_current_index >= m_input.size();
        }
};
#endif