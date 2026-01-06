#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "RuntimeValue.h"
#include "Environment.h"
#include <vector>
#include <memory>

class Interpreter : public Visitor {
public:
    Interpreter();
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);

    // Expression Visitors
    std::any visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) override;
    std::any visitVariableExpr(std::shared_ptr<VariableExpr> expr) override;
    std::any visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) override;
    std::any visitAssignmentExpr(std::shared_ptr<AssignmentExpr> expr) override;
    std::any visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) override;
    std::any visitCallExpr(std::shared_ptr<CallExpr> expr) override;
    std::any visitGetExpr(std::shared_ptr<GetExpr> expr) override;
    std::any visitArrayLiteralExpr(std::shared_ptr<ArrayLiteralExpr> expr) override;
    std::any visitIndexExpr(std::shared_ptr<IndexExpr> expr) override;
    std::any visitArrayAssignmentExpr(std::shared_ptr<ArrayAssignmentExpr> expr) override;

    // Statement Visitors
    std::any visitExprStmt(std::shared_ptr<ExprStmt> stmt) override;
    std::any visitPrintStmt(std::shared_ptr<PrintStmt> stmt) override;
    std::any visitLetStmt(std::shared_ptr<LetStmt> stmt) override;
    std::any visitBlockStmt(std::shared_ptr<BlockStmt> stmt) override;
    std::any visitIfStmt(std::shared_ptr<IfStmt> stmt) override;
    std::any visitWhileStmt(std::shared_ptr<WhileStmt> stmt) override;
    std::any visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) override;
    std::any visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
    std::any visitModelStmt(std::shared_ptr<ModelStmt> stmt) override;

    void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);

private:
    std::shared_ptr<Environment> environment;

    RuntimeValue evaluate(std::shared_ptr<Expr> expr);
    void execute(std::shared_ptr<Stmt> stmt);

    bool isTruthy(const RuntimeValue& value);
    bool isEqual(const RuntimeValue& a, const RuntimeValue& b);
    void checkNumberOperand(const Token& operatorToken, const RuntimeValue& operand);
    void checkNumberOperands(const Token& operatorToken, const RuntimeValue& left, const RuntimeValue& right);
};

#endif // INTERPRETER_H
