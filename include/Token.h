#ifndef TOKEN_H
#define TOKEN_H

#include "TokenType.h"
#include <string>
#include <variant>

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, int, std::string> literal; // Usually just int for this grammar, but keeping flexible
    int line;

    Token(TokenType type, std::string lexeme, std::variant<std::monostate, int, std::string> literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

    std::string toString() const {
        return "Token(" + std::to_string(static_cast<int>(type)) + ", " + lexeme + ")";
    }
};

#endif // TOKEN_H
