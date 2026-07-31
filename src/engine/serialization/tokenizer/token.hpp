#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string_view>
#include <cstdint>

enum class TokenType{
    EndOfFile,
    Identifier,
    Number, // Float or Int
    String, // String value
    EQUALS, // :
    LeftBrace, // {
    RightBrace, // }
    INVALID // Invalid token
};

struct Token{
    TokenType token_type;
    std::string_view token_value;
    std::uint32_t line{1};
    std::uint32_t column{1};
};

#endif