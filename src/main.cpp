#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/AST.h"

// Simple AST Printer Visitor
class AstPrinter : public Visitor {
public:
    std::string print(const std::vector<std::shared_ptr<Stmt>>& statements) {
        std::string result;
        for (const auto& stmt : statements) {
            result += std::any_cast<std::string>(stmt->accept(this)) + "\n";
        }
        return result;
    }

    std::any visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) override {
        return parenthesize(expr->op.lexeme, {expr->left, expr->right});
    }

    std::any visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) override {
        return parenthesize(expr->op.lexeme, {expr->right});
    }

    std::any visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) override {
        if (std::holds_alternative<int>(expr->value)) {
            return std::to_string(std::get<int>(expr->value));
        } else if (std::holds_alternative<std::string>(expr->value)) {
            return "\"" + std::get<std::string>(expr->value) + "\"";
        }
        return std::string("nil");
    }

    std::any visitVariableExpr(std::shared_ptr<VariableExpr> expr) override {
        return expr->name.lexeme;
    }

    std::any visitCallExpr(std::shared_ptr<CallExpr> expr) override {
        std::string result = "(call " + std::any_cast<std::string>(expr->callee->accept(this));
        for (const auto& arg : expr->arguments) {
            result += " " + std::any_cast<std::string>(arg->accept(this));
        }
        result += ")";
        return result;
    }

    std::any visitAssignmentExpr(std::shared_ptr<AssignmentExpr> expr) override {
        return parenthesize("= " + expr->name.lexeme, {expr->value});
    }

    std::any visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) override {
        return parenthesize(expr->op.lexeme, {expr->left, expr->right});
    }

    std::any visitBlockStmt(std::shared_ptr<BlockStmt> stmt) override {
        std::string result = "(block";
        for (const auto& s : stmt->statements) {
            result += " " + std::any_cast<std::string>(s->accept(this));
        }
        result += ")";
        return result;
    }

    std::any visitLetStmt(std::shared_ptr<LetStmt> stmt) override {
        std::string result = "(let " + stmt->name.lexeme;
        if (stmt->initializer) {
            result += " " + std::any_cast<std::string>(stmt->initializer->accept(this));
        }
        result += ")";
        return result;
    }

    std::any visitIfStmt(std::shared_ptr<IfStmt> stmt) override {
        std::string result = "(if " + std::any_cast<std::string>(stmt->condition->accept(this));
        result += " " + std::any_cast<std::string>(stmt->thenBranch->accept(this));
        if (stmt->elseBranch) {
            result += " " + std::any_cast<std::string>(stmt->elseBranch->accept(this));
        }
        result += ")";
        return result;
    }

    std::any visitWhileStmt(std::shared_ptr<WhileStmt> stmt) override {
        return "(while " + std::any_cast<std::string>(stmt->condition->accept(this)) + 
               " " + std::any_cast<std::string>(stmt->body->accept(this)) + ")";
    }

    std::any visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override {
        std::string result = "(return";
        if (stmt->value) {
            result += " " + std::any_cast<std::string>(stmt->value->accept(this));
        }
        result += ")";
        return result;
    }

    std::any visitPrintStmt(std::shared_ptr<PrintStmt> stmt) override {
        return "(print " + std::any_cast<std::string>(stmt->expression->accept(this)) + ")";
    }

    std::any visitExprStmt(std::shared_ptr<ExprStmt> stmt) override {
        return std::any_cast<std::string>(stmt->expression->accept(this));
    }

    std::any visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) override {
        std::string result = "(fn " + stmt->name.lexeme + " (";
        for (const auto& param : stmt->params) {
            result += param.lexeme + " ";
        }
        result += ") ";
        for (const auto& s : stmt->body) {
            result += std::any_cast<std::string>(s->accept(this));
        }
        result += ")";
        return result;
    }

private:
    std::string parenthesize(std::string name, std::vector<std::shared_ptr<Expr>> exprs) {
        std::string result = "(" + name;
        for (const auto& expr : exprs) {
            result += " " + std::any_cast<std::string>(expr->accept(this));
        }
        result += ")";
        return result;
    }
};

void run(std::string source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    AstPrinter printer;
    std::cout << printer.print(statements) << std::endl;
}

void runFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << path << std::endl;
        exit(74);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: trolllang <script>" << std::endl;
        return 64;
    }
    runFile(argv[1]);
    return 0;
}
