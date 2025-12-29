#ifndef AST_H
#define AST_H

#include "Token.h"
#include <vector>
#include <memory>
#include <optional>
#include <any>

// Forward declarations
struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct VariableExpr;
struct CallExpr;
struct AssignmentExpr;
struct LogicalExpr;
struct ArrayLiteralExpr;

struct BlockStmt;
struct LetStmt;
struct IfStmt;
struct WhileStmt;
struct ReturnStmt;
struct PrintStmt;
struct ExprStmt;
struct FunctionStmt;

class Visitor {
public:
    virtual ~Visitor() = default;

    // Expressions
    virtual std::any visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) = 0;
    virtual std::any visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) = 0;
    virtual std::any visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) = 0;
    virtual std::any visitVariableExpr(std::shared_ptr<VariableExpr> expr) = 0;
    virtual std::any visitCallExpr(std::shared_ptr<CallExpr> expr) = 0;
    virtual std::any visitAssignmentExpr(std::shared_ptr<AssignmentExpr> expr) = 0;
    virtual std::any visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) = 0;
    virtual std::any visitArrayLiteralExpr(std::shared_ptr<ArrayLiteralExpr> expr) = 0;

    // Statements
    virtual std::any visitBlockStmt(std::shared_ptr<BlockStmt> stmt) = 0;
    virtual std::any visitLetStmt(std::shared_ptr<LetStmt> stmt) = 0;
    virtual std::any visitIfStmt(std::shared_ptr<IfStmt> stmt) = 0;
    virtual std::any visitWhileStmt(std::shared_ptr<WhileStmt> stmt) = 0;
    virtual std::any visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) = 0;
    virtual std::any visitPrintStmt(std::shared_ptr<PrintStmt> stmt) = 0;
    virtual std::any visitExprStmt(std::shared_ptr<ExprStmt> stmt) = 0;
    virtual std::any visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) = 0;
};

struct Expr {
    virtual ~Expr() = default;
    virtual std::any accept(Visitor* visitor) = 0;
};

struct Stmt {
    virtual ~Stmt() = default;
    virtual std::any accept(Visitor* visitor) = 0;
};

// --- Expressions ---

struct BinaryExpr : public Expr, public std::enable_shared_from_this<BinaryExpr> {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    BinaryExpr(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitBinaryExpr(shared_from_this());
    }
};

struct UnaryExpr : public Expr, public std::enable_shared_from_this<UnaryExpr> {
    Token op;
    std::shared_ptr<Expr> right;

    UnaryExpr(Token op, std::shared_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitUnaryExpr(shared_from_this());
    }
};

struct LiteralExpr : public Expr, public std::enable_shared_from_this<LiteralExpr> {
    std::variant<std::monostate, int, double, std::string> value;

    LiteralExpr(std::variant<std::monostate, int, double, std::string> value)
        : value(std::move(value)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitLiteralExpr(shared_from_this());
    }
};

struct VariableExpr : public Expr, public std::enable_shared_from_this<VariableExpr> {
    Token name;

    VariableExpr(Token name) : name(std::move(name)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitVariableExpr(shared_from_this());
    }
};

struct CallExpr : public Expr, public std::enable_shared_from_this<CallExpr> {
    std::shared_ptr<Expr> callee;
    Token paren; // For error reporting
    std::vector<std::shared_ptr<Expr>> arguments;

    CallExpr(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitCallExpr(shared_from_this());
    }
};

struct AssignmentExpr : public Expr, public std::enable_shared_from_this<AssignmentExpr> {
    Token name;
    std::shared_ptr<Expr> value;

    AssignmentExpr(Token name, std::shared_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitAssignmentExpr(shared_from_this());
    }
};

struct LogicalExpr : public Expr, public std::enable_shared_from_this<LogicalExpr> {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    LogicalExpr(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitLogicalExpr(shared_from_this());
    }
};

struct ArrayLiteralExpr : public Expr, public std::enable_shared_from_this<ArrayLiteralExpr> {
    std::vector<std::shared_ptr<Expr>> elements;

    ArrayLiteralExpr(std::vector<std::shared_ptr<Expr>> elements)
        : elements(std::move(elements)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitArrayLiteralExpr(shared_from_this());
    }
};

// --- Statements ---

struct BlockStmt : public Stmt, public std::enable_shared_from_this<BlockStmt> {
    std::vector<std::shared_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::shared_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitBlockStmt(shared_from_this());
    }
};

struct LetStmt : public Stmt, public std::enable_shared_from_this<LetStmt> {
    Token name;
    std::shared_ptr<Expr> initializer; // Can be null

    LetStmt(Token name, std::shared_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitLetStmt(shared_from_this());
    }
};

struct IfStmt : public Stmt, public std::enable_shared_from_this<IfStmt> {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch; // Can be null

    IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitIfStmt(shared_from_this());
    }
};

struct WhileStmt : public Stmt, public std::enable_shared_from_this<WhileStmt> {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;

    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitWhileStmt(shared_from_this());
    }
};

struct ReturnStmt : public Stmt, public std::enable_shared_from_this<ReturnStmt> {
    Token keyword;
    std::shared_ptr<Expr> value; // Can be null

    ReturnStmt(Token keyword, std::shared_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitReturnStmt(shared_from_this());
    }
};

struct PrintStmt : public Stmt, public std::enable_shared_from_this<PrintStmt> {
    std::shared_ptr<Expr> expression;

    PrintStmt(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitPrintStmt(shared_from_this());
    }
};

struct ExprStmt : public Stmt, public std::enable_shared_from_this<ExprStmt> {
    std::shared_ptr<Expr> expression;

    ExprStmt(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitExprStmt(shared_from_this());
    }
};

struct FunctionStmt : public Stmt, public std::enable_shared_from_this<FunctionStmt> {
    Token name;
    std::vector<Token> params;
    std::vector<std::shared_ptr<Stmt>> body; 

    FunctionStmt(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

    std::any accept(Visitor* visitor) override {
        return visitor->visitFunctionStmt(shared_from_this());
    }
};

#endif // AST_H
