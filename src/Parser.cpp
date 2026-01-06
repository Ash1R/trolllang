#include "../include/Parser.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (ParseError& error) {
            synchronize();
        }
    }
    return statements;
}

std::shared_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::MODEL})) return modelDeclaration();
    if (check(TokenType::FN)) return function();
    
    return statement();
}

std::shared_ptr<Stmt> Parser::modelDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expected model name.");
    consume(TokenType::LEFT_BRACE, "Expected '{' before model body.");

    std::vector<std::shared_ptr<Stmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match({TokenType::LET})) {
             methods.push_back(letDeclaration());
        } else if (check(TokenType::FN)) {
             methods.push_back(function());
        } else {
            throw error(peek(), "Expected 'let' or 'fn' inside model.");
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after model body.");
    return std::make_shared<ModelStmt>(name, methods);
}

std::shared_ptr<FunctionStmt> Parser::function() {
    consume(TokenType::FN, "Part of grammar: program := (function)* EOF. Expected 'fn' to start a function.");
    
    Token name = consume(TokenType::IDENTIFIER, "Expected function name.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name.");
    
    std::vector<Token> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expected parameter name."));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters.");
    
    consume(TokenType::LEFT_BRACE, "Expected '{' before function body.");
    std::shared_ptr<BlockStmt> bodyBlock = block();
    
    // Convert BlockStmt statements to vector for FunctionStmt
    return std::make_shared<FunctionStmt>(name, parameters, bodyBlock->statements);
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match({TokenType::LET})) return letDeclaration();
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::PRINT})) return printStatement();
    if (match({TokenType::LEFT_BRACE})) return block();

    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::letDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name.");
    std::shared_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
    return std::make_shared<LetStmt>(name, initializer);
}

std::shared_ptr<BlockStmt> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");
    return std::make_shared<BlockStmt>(statements);
}

std::shared_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition.");

    std::shared_ptr<Stmt> thenBranch = statement();
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition.");
    std::shared_ptr<Stmt> body = statement();

    return std::make_shared<WhileStmt>(condition, body);
}

std::shared_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::shared_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after return value.");
    return std::make_shared<ReturnStmt>(keyword, value);
}

std::shared_ptr<Stmt> Parser::printStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'print'.");
    std::shared_ptr<Expr> value = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after print value.");
    consume(TokenType::SEMICOLON, "Expected ';' after print statement."); // Grammar says: printStmt := "print" "(" expr ")" ";"
    return std::make_shared<PrintStmt>(value);
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    std::shared_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression.");
    return std::make_shared<ExprStmt>(expr);
}

// Expressions

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    std::shared_ptr<Expr> expr = logicOr();

    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::shared_ptr<Expr> value = assignment(); // Right-associative

        if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr)) {
            Token name = varExpr->name;
            return std::make_shared<AssignmentExpr>(name, value);
        } else if (auto indexExpr = std::dynamic_pointer_cast<IndexExpr>(expr)) {
            return std::make_shared<ArrayAssignmentExpr>(indexExpr->object, indexExpr->index, value, indexExpr->bracket);
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::shared_ptr<Expr> Parser::logicOr() {
    std::shared_ptr<Expr> expr = logicAnd();

    while (match({TokenType::PIPE_PIPE})) {
        Token op = previous();
        std::shared_ptr<Expr> right = logicAnd();
        expr = std::make_shared<LogicalExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::logicAnd() {
    std::shared_ptr<Expr> expr = equality();

    while (match({TokenType::AMPERSAND_AMPERSAND})) {
        Token op = previous();
        std::shared_ptr<Expr> right = equality();
        expr = std::make_shared<LogicalExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    std::shared_ptr<Expr> expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    std::shared_ptr<Expr> expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    std::shared_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = factor();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::factor() {
    std::shared_ptr<Expr> expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT, TokenType::AT})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<UnaryExpr>(op, right);
    }

    return call();
}

std::shared_ptr<Expr> Parser::call() {
    std::shared_ptr<Expr> expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            // arguments
            std::vector<std::shared_ptr<Expr>> arguments;
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    arguments.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            Token paren = consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments.");
            expr = std::make_shared<CallExpr>(expr, paren, arguments);
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_shared<GetExpr>(expr, name);
        } else if (match({TokenType::LEFT_BRACKET})) {
            std::shared_ptr<Expr> index = expression();
            Token bracket = consume(TokenType::RIGHT_BRACKET, "Expected ']' after index.");
            expr = std::make_shared<IndexExpr>(expr, index, bracket);
        } else {
            break;
        }
    }


    return expr;
}

std::shared_ptr<Expr> Parser::primary() {
    if (match({TokenType::NUMBER})) {
        return std::make_shared<LiteralExpr>(previous().literal);
    }

    if (match({TokenType::STRING})) {
        std::string value = std::get<std::string>(previous().literal);
        return std::make_shared<LiteralExpr>(value);
    }

    if (match({TokenType::TRUE})) return std::make_shared<LiteralExpr>(true);
    if (match({TokenType::FALSE})) return std::make_shared<LiteralExpr>(false);

    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<VariableExpr>(previous());
    }

    if (match({TokenType::LEFT_BRACKET})) {
        return arrayLiteral();
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
        return expr;
    }

    throw error(peek(), "Expect expression.");
}

std::shared_ptr<Expr> Parser::arrayLiteral() {
    std::vector<std::shared_ptr<Expr>> elements;
    if (!check(TokenType::RIGHT_BRACKET)) {
        do {
            elements.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_BRACKET, "Expected ']' after array elements.");
    return std::make_shared<ArrayLiteralExpr>(elements);
}

// Helpers

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::consume(TokenType type, std::string message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::FN:
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                ;
        }
        advance();
    }
}

Parser::ParseError Parser::error(Token token, std::string message) {
    std::cerr << "[Line " << token.line << "] Error at '" << token.lexeme << "': " << message << "\n";
    return ParseError(message);
}
