#ifndef TROLL_INSTANCE_H
#define TROLL_INSTANCE_H

#include <memory>
#include <string>
#include <unordered_map>
#include "RuntimeValue.h"
#include "Environment.h"
#include "Token.h"

// Forward declare TrollModel
class TrollModel;

class TrollInstance {
public:
    std::shared_ptr<TrollModel> model;
    std::shared_ptr<Environment> env;

    TrollInstance(std::shared_ptr<TrollModel> model); // Defined in CPP to avoid circular dep

    RuntimeValue get(Token name);
    void set(Token name, RuntimeValue value);
};

#endif // TROLL_INSTANCE_H
