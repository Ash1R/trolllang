#ifndef TROLL_MODEL_H
#define TROLL_MODEL_H

#include "Callable.h"
#include "TrollInstance.h"
#include "Interpreter.h" // Needed for executeBlock logic access via friend or similar? 
// Actually TrollModel needs to execute body. 
// Ideally TrollModel just holds the Stmt and Interpreter visits it?
// Or TrollModel::call uses Interpreter.

class TrollModel : public Callable {
public:
    std::string name;
    std::shared_ptr<ModelStmt> declaration;
    std::shared_ptr<Environment> closure;

    TrollModel(std::shared_ptr<ModelStmt> declaration, std::shared_ptr<Environment> closure)
        : name(declaration->name.lexeme), declaration(std::move(declaration)), closure(std::move(closure)) {}

    int arity() override {
        return 0; // Default constructor 0 args for now
    }

    RuntimeValue call(Interpreter* interpreter, const std::vector<RuntimeValue>& arguments) override;

    std::string toString() override {
        return name;
    }
};

#endif // TROLL_MODEL_H
