#ifndef TROLL_FUNCTION_H
#define TROLL_FUNCTION_H

#include "Callable.h"
#include "AST.h"
#include "Environment.h"
#include "Interpreter.h"
#include "Return.h"

class TrollFunction : public Callable {
public:
    std::shared_ptr<FunctionStmt> declaration;
    std::shared_ptr<Environment> closure;

    TrollFunction(std::shared_ptr<FunctionStmt> declaration, std::shared_ptr<Environment> closure)
        : declaration(std::move(declaration)), closure(std::move(closure)) {}

    int arity() override {
        return declaration->params.size();
    }

    RuntimeValue call(Interpreter* interpreter, const std::vector<RuntimeValue>& arguments) override {
        // Create a new environment for the function scope
        std::shared_ptr<Environment> environment = std::make_shared<Environment>(closure);

        for (size_t i = 0; i < declaration->params.size(); ++i) {
            environment->define(declaration->params[i].lexeme, arguments[i]);
        }

        try {
            interpreter->executeBlock(declaration->body, environment);
        } catch (const Return& returnValue) {
            return returnValue.value;
        }

        return RuntimeValue(std::monostate{}); // Default return nil
    }

    std::string toString() override {
        return "<fn " + declaration->name.lexeme + ">";
    }
};

#endif // TROLL_FUNCTION_H
