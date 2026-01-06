#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "RuntimeValue.h"
#include "Token.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>

class RuntimeError : public std::runtime_error {
public:
    Token token;
    RuntimeError(Token token, std::string message) 
        : std::runtime_error(message), token(token) {}
};

class Environment {
public:
    Environment() : enclosing(nullptr) {}
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

    void define(const std::string& name, RuntimeValue value) {
        values[name] = value;
    }

    RuntimeValue get(Token name) {
        if (values.count(name.lexeme)) {
            return values[name.lexeme];
        }

        if (enclosing != nullptr) {
            return enclosing->get(name);
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    RuntimeValue getAt(const std::string& name) {
        if (values.count(name)) {
            return values[name];
        }
        return RuntimeValue(std::monostate{}); 
    }

    void assign(Token name, RuntimeValue value) {
        if (values.count(name.lexeme)) {
            values[name.lexeme] = value;
            return;
        }

        if (enclosing != nullptr) {
            enclosing->assign(name, value);
            return;
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    std::shared_ptr<Environment> enclosing;
private:
    std::unordered_map<std::string, RuntimeValue> values;
};

#endif // ENVIRONMENT_H
