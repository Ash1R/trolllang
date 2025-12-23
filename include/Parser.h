#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& msg) : std::runtime_error(msg) {}
    };

    Parser(std::vector<Token> tokens);
    std::vector<std::shared_ptr<Stmt>> parse();

private:
    std::vector<Token> tokens;
    int current = 0;

    // Grammar rules
    std::shared_ptr<Stmt> declaration(); // Covers function and standard statements if we were allowing globals, but grammar says program := function*
    std::shared_ptr<FunctionStmt> function();
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> letDeclaration();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> whileStatement();
    std::shared_ptr<Stmt> returnStatement();
    std::shared_ptr<Stmt> printStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::shared_ptr<BlockStmt> block();

    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> logicOr();
    std::shared_ptr<Expr> logicAnd();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> primary();

    // Helpers
    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    Token consume(TokenType type, std::string message);
    void synchronize();
    ParseError error(Token token, std::string message);
};

#endif // PARSER_H
