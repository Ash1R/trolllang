#ifndef TOKENTYPE_H
#define TOKENTYPE_H

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, SEMICOLON, MINUS, PLUS, SLASH, STAR, PERCENT, AT, DOT,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    AMPERSAND_AMPERSAND, PIPE_PIPE,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    FN, LET, IF, ELSE, WHILE, RETURN, PRINT, MODEL,
    TRUE, FALSE,

    // End of file
    END_OF_FILE
};

#endif // TOKENTYPE_H
