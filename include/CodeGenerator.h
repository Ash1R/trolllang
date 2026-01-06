#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "AST.h"
#include <map>
#include <string>
#include <memory>
#include <vector>

// LLVM Includes
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

class CodeGenerator : public Visitor {
public:
    CodeGenerator();
    void generateCode(const std::vector<std::shared_ptr<Stmt>>& statements);
    void saveModule(const std::string& filename);

    void setupExternalFunctions(); // Helper to declare printf

    // Visitor Implementation
    std::any visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) override;
    std::any visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) override;
    std::any visitVariableExpr(std::shared_ptr<VariableExpr> expr) override;
    std::any visitCallExpr(std::shared_ptr<CallExpr> expr) override;
    std::any visitGetExpr(std::shared_ptr<GetExpr> expr) override;
    std::any visitAssignmentExpr(std::shared_ptr<AssignmentExpr> expr) override;
    std::any visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) override;
    std::any visitArrayLiteralExpr(std::shared_ptr<ArrayLiteralExpr> expr) override;

    std::any visitBlockStmt(std::shared_ptr<BlockStmt> stmt) override;
    std::any visitLetStmt(std::shared_ptr<LetStmt> stmt) override;
    std::any visitIfStmt(std::shared_ptr<IfStmt> stmt) override;
    std::any visitWhileStmt(std::shared_ptr<WhileStmt> stmt) override;
    std::any visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
    std::any visitPrintStmt(std::shared_ptr<PrintStmt> stmt) override;
    std::any visitExprStmt(std::shared_ptr<ExprStmt> stmt) override;
    std::any visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) override;
    std::any visitModelStmt(std::shared_ptr<ModelStmt> stmt) override;

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    std::map<std::string, llvm::AllocaInst*> namedValues;

    llvm::Value* evaluate(std::shared_ptr<Expr> expr);
    
    // Type Support
    llvm::StructType* valueStructType; // { i32 type, double num, ptr ptr }
    llvm::Value* createNumber(double val);
    llvm::Value* createNumberFromValue(llvm::Value* val);
    llvm::Value* createBool(bool val);
    llvm::Value* createArray(llvm::Value* ptr); // ptr is i8*
    
    // Helpers
    llvm::Value* unpackNumber(llvm::Value* trollVal);
    
    enum ValueType {
        TYPE_NUMBER = 0,
        TYPE_ARRAY = 1,
        TYPE_BOOL = 2
    };
};

#endif // CODE_GENERATOR_H
