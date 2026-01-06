#include "../include/Interpreter.h"
#include "../include/TrollFunction.h"
#include "../include/TrollArray.h"
#include "../include/TrollInstance.h"
#include "../include/TrollModel.h"
#include "../include/Return.h"
#include <iostream>
#include <cmath>

Interpreter::Interpreter() {
    environment = std::make_shared<Environment>();
}

void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements) {
    try {
        for (const auto& stmt : statements) {
            execute(stmt);
        }
    } catch (RuntimeError& error) {
        std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
    }
}

RuntimeValue Interpreter::evaluate(std::shared_ptr<Expr> expr) {
    return std::any_cast<RuntimeValue>(expr->accept(this));
}

void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
    stmt->accept(this);
}

void Interpreter::executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> env) {
    std::shared_ptr<Environment> previous = this->environment;
    this->environment = env;

    try {
        for (const auto& stmt : statements) {
            execute(stmt);
        }
    } catch (...) {
        this->environment = previous;
        throw;
    }

    this->environment = previous;
}

// Visitors

std::any Interpreter::visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) {
    if (std::holds_alternative<int>(expr->value)) {
        return RuntimeValue(static_cast<double>(std::get<int>(expr->value))); // Treat ints as doubles runtime
    }
    if (std::holds_alternative<double>(expr->value)) {
        return RuntimeValue(std::get<double>(expr->value));
    }
    if (std::holds_alternative<std::string>(expr->value)) {
        return RuntimeValue(std::get<std::string>(expr->value));
    }
    if (std::holds_alternative<bool>(expr->value)) {
        return RuntimeValue(std::get<bool>(expr->value));
    }
    return RuntimeValue(std::monostate{}); // Nil
}

std::any Interpreter::visitVariableExpr(std::shared_ptr<VariableExpr> expr) {
    return environment->get(expr->name);
}

std::any Interpreter::visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) {
    RuntimeValue left = evaluate(expr->left);
    RuntimeValue right = evaluate(expr->right);

    switch (expr->op.type) {
        case TokenType::MINUS:
            checkNumberOperands(expr->op, left, right);
            return RuntimeValue(std::get<double>(left) - std::get<double>(right));
        case TokenType::PLUS:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return RuntimeValue(std::get<double>(left) + std::get<double>(right));
            }
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                return RuntimeValue(std::get<std::string>(left) + std::get<std::string>(right));
            }
             throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
        case TokenType::SLASH:
            checkNumberOperands(expr->op, left, right);
            return RuntimeValue(std::get<double>(left) / std::get<double>(right));
        case TokenType::STAR:
            checkNumberOperands(expr->op, left, right);
            return RuntimeValue(std::get<double>(left) * std::get<double>(right));
        case TokenType::GREATER:
            checkNumberOperands(expr->op, left, right);
            return RuntimeValue(std::get<double>(left) > std::get<double>(right));
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr->op, left, right);
            return RuntimeValue(std::get<double>(left) >= std::get<double>(right));
        case TokenType::LESS:
            checkNumberOperands(expr->op, left, right);
            return RuntimeValue(std::get<double>(left) < std::get<double>(right));
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr->op, left, right);
            return RuntimeValue(std::get<double>(left) <= std::get<double>(right));
        case TokenType::AT:
            // Matrix Multiplication
            if (std::holds_alternative<std::shared_ptr<TrollArray>>(left) && std::holds_alternative<std::shared_ptr<TrollArray>>(right)) {
                 // Simplified MatMul implementation
                 // Assuming 2D matrices of doubles for now.
                 // Real implementation would need comprehensive error checking and support for 1D/Tensor ops.
                 auto lArr = std::get<std::shared_ptr<TrollArray>>(left);
                 auto rArr = std::get<std::shared_ptr<TrollArray>>(right);
                 
                 // Implement MatMul Logic here (basic naive O(n^3))
                 // Rows of Left
                 size_t rowsL = lArr->elements.size();
                 if (rowsL == 0) throw RuntimeError(expr->op, "Empty matrix.");

                 // Check Left[0] to see if it's 2D
                 if (!std::holds_alternative<std::shared_ptr<TrollArray>>(lArr->elements[0])) {
                     throw RuntimeError(expr->op, "MatMul only supports 2D matrices for now.");
                 }
                 auto lRow0 = std::get<std::shared_ptr<TrollArray>>(lArr->elements[0]);
                 size_t colsL = lRow0->elements.size();

                 // Rows of Right
                 size_t rowsR = rArr->elements.size();
                 if (rowsR != colsL) throw RuntimeError(expr->op, "Matrix dimensions mismatch.");

                 // Check Right[0]
                 if (rowsR == 0) throw RuntimeError(expr->op, "Empty matrix.");
                 if (!std::holds_alternative<std::shared_ptr<TrollArray>>(rArr->elements[0])) {
                      throw RuntimeError(expr->op, "MatMul only supports 2D matrices for now.");
                 }
                 auto rRow0 = std::get<std::shared_ptr<TrollArray>>(rArr->elements[0]);
                 size_t colsR = rRow0->elements.size();

                 // Result Matrix: rowsL x colsR
                 std::vector<RuntimeValue> resRows;
                 
                 for (size_t i = 0; i < rowsL; ++i) {
                     std::vector<RuntimeValue> newRow;
                     for (size_t j = 0; j < colsR; ++j) {
                         double sum = 0.0;
                         for (size_t k = 0; k < colsL; ++k) {
                             // Val A = Left[i][k]
                             auto rowVal = std::get<std::shared_ptr<TrollArray>>(lArr->elements[i]);
                             double valA = std::get<double>(rowVal->elements[k]);
                             
                             // Val B = Right[k][j]
                             auto colVal = std::get<std::shared_ptr<TrollArray>>(rArr->elements[k]);
                             double valB = std::get<double>(colVal->elements[j]);

                             sum += valA * valB;
                         }
                         newRow.push_back(RuntimeValue(sum));
                     }
                     resRows.push_back(std::make_shared<TrollArray>(newRow)); // Row is Array
                 }
                 return RuntimeValue(std::make_shared<TrollArray>(resRows));
            }
            throw RuntimeError(expr->op, "MatMul operator '@' requires two TrollArray operands.");
        case TokenType::BANG_EQUAL:
            return RuntimeValue(!isEqual(left, right));
        case TokenType::EQUAL_EQUAL:
            return RuntimeValue(isEqual(left, right));
        default:
            return RuntimeValue(std::monostate{});
    }
}

std::any Interpreter::visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) {
    RuntimeValue right = evaluate(expr->right);

    switch (expr->op.type) {
        case TokenType::MINUS:
            checkNumberOperand(expr->op, right);
            return RuntimeValue(-std::get<double>(right));
        case TokenType::BANG:
            return RuntimeValue(!isTruthy(right));
        default:
            return RuntimeValue(std::monostate{});
    }
}

std::any Interpreter::visitAssignmentExpr(std::shared_ptr<AssignmentExpr> expr) {
    RuntimeValue value = evaluate(expr->value);
    environment->assign(expr->name, value);
    return value;
}

std::any Interpreter::visitExprStmt(std::shared_ptr<ExprStmt> stmt) {
    evaluate(stmt->expression);
    return std::any();
}

std::any Interpreter::visitPrintStmt(std::shared_ptr<PrintStmt> stmt) {
    RuntimeValue value = evaluate(stmt->expression);
    std::cout << to_string(value) << "\n";
    return std::any();
}

std::any Interpreter::visitLetStmt(std::shared_ptr<LetStmt> stmt) {
    RuntimeValue value = std::monostate{};
    if (stmt->initializer != nullptr) {
        value = evaluate(stmt->initializer);
    }
    environment->define(stmt->name.lexeme, value);
    return std::any();
}

std::any Interpreter::visitBlockStmt(std::shared_ptr<BlockStmt> stmt) {
    executeBlock(stmt->statements, std::make_shared<Environment>(environment));
    return std::any();
}

// Stubs / Phase 2+ features

std::any Interpreter::visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) {
    RuntimeValue left = evaluate(expr->left);

    if (expr->op.type == TokenType::PIPE_PIPE) {
        if (isTruthy(left)) return left;
    } else {
        if (!isTruthy(left)) return left;
    }

    return evaluate(expr->right);
}

std::any Interpreter::visitCallExpr(std::shared_ptr<CallExpr> expr) {
    RuntimeValue callee = evaluate(expr->callee);

    std::vector<RuntimeValue> arguments;
    for (const auto& argument : expr->arguments) {
        arguments.push_back(evaluate(argument));
    }

    if (!std::holds_alternative<std::shared_ptr<Callable>>(callee)) {
        throw RuntimeError(expr->paren, "Can only call functions and classes.");
    }

    std::shared_ptr<Callable> function = std::get<std::shared_ptr<Callable>>(callee);

    if (arguments.size() != function->arity()) {
        throw RuntimeError(expr->paren, "Expected " + 
            std::to_string(function->arity()) + " arguments but got " + 
            std::to_string(arguments.size()) + ".");
    }

    return function->call(this, arguments);
}

std::any Interpreter::visitGetExpr(std::shared_ptr<GetExpr> expr) {
    RuntimeValue object = evaluate(expr->object);
    if (std::holds_alternative<std::shared_ptr<TrollInstance>>(object)) {
        return std::get<std::shared_ptr<TrollInstance>>(object)->get(expr->name);
    }

    throw RuntimeError(expr->name, "Only instances have properties.");
}

std::any Interpreter::visitArrayLiteralExpr(std::shared_ptr<ArrayLiteralExpr> expr) {
    std::vector<RuntimeValue> elements;
    for (const auto& el : expr->elements) {
        elements.push_back(evaluate(el));
    }
    return RuntimeValue(std::make_shared<TrollArray>(elements));
}

std::any Interpreter::visitIfStmt(std::shared_ptr<IfStmt> stmt) {
    if (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->thenBranch);
    } else if (stmt->elseBranch != nullptr) {
        execute(stmt->elseBranch);
    }
    return std::any();
}

std::any Interpreter::visitWhileStmt(std::shared_ptr<WhileStmt> stmt) {
    while (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->body);
    }
    return std::any();
}

std::any Interpreter::visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) {
    auto function = std::make_shared<TrollFunction>(stmt, environment);
    environment->define(stmt->name.lexeme, RuntimeValue(function));
    return std::any();
}

std::any Interpreter::visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) {
    RuntimeValue value = std::monostate{};
    if (stmt->value != nullptr) {
        value = evaluate(stmt->value);
    }

    throw Return(value);
}

std::any Interpreter::visitModelStmt(std::shared_ptr<ModelStmt> stmt) {
    auto model = std::make_shared<TrollModel>(stmt, environment);
    environment->define(stmt->name.lexeme, RuntimeValue(model));
    return std::any();
}

// Helpers

bool Interpreter::isTruthy(const RuntimeValue& value) {
    if (std::holds_alternative<std::monostate>(value)) return false;
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
    return true;
}

bool Interpreter::isEqual(const RuntimeValue& a, const RuntimeValue& b) {
    // Simplified equality
    if (a.index() != b.index()) return false;
    if (std::holds_alternative<double>(a)) return std::get<double>(a) == std::get<double>(b);
    if (std::holds_alternative<bool>(a)) return std::get<bool>(a) == std::get<bool>(b);
    if (std::holds_alternative<std::string>(a)) return std::get<std::string>(a) == std::get<std::string>(b);
    return true; // nil == nil
}

void Interpreter::checkNumberOperand(const Token& operatorToken, const RuntimeValue& operand) {
    if (std::holds_alternative<double>(operand)) return;
    throw RuntimeError(operatorToken, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token& operatorToken, const RuntimeValue& left, const RuntimeValue& right) {
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) return;
    throw RuntimeError(operatorToken, "Operands must be numbers.");
}
