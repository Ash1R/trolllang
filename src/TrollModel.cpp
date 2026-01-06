#include "../include/TrollModel.h"
#include "../include/TrollInstance.h"
#include "../include/Interpreter.h"

// TrollInstance implementation
TrollInstance::TrollInstance(std::shared_ptr<TrollModel> model) : model(std::move(model)) {}

RuntimeValue TrollInstance::get(Token name) {
    RuntimeValue val = env->getAt(name.lexeme);
    if (!std::holds_alternative<std::monostate>(val)) {
        return val;
    }
    // TODO: Look up methods in model? For now we define methods in instance env as closures.
    
    throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void TrollInstance::set(Token name, RuntimeValue value) {
    env->define(name.lexeme, value); // Or assign? define allows creating new fields?
    // For now, allow defining new fields or overwriting.
}

// TrollModel implementation
RuntimeValue TrollModel::call(Interpreter* interpreter, const std::vector<RuntimeValue>& arguments) {
    auto instance = std::make_shared<TrollInstance>(std::make_shared<TrollModel>(*this));
    
    // Create instance environment
    instance->env = std::make_shared<Environment>(closure);
    
    interpreter->executeBlock(declaration->methods, instance->env);
    
    return std::shared_ptr<TrollInstance>(instance);
}
