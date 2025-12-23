#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <vector>
#include <string>
#include <unordered_map>

class Lexer {
public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    static const std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, std::variant<std::monostate, int, std::string> literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    bool isDigit(char c);
};

#endif // LEXER_H
