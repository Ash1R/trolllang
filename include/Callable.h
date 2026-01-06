#ifndef CALLABLE_H
#define CALLABLE_H

#include <vector>
#include <string>
#include <memory>
// Forward declare RuntimeValue
#include "RuntimeValue.h"

class Interpreter;

class Callable {
public:
    virtual ~Callable() = default;
    virtual int arity() = 0;
    virtual RuntimeValue call(Interpreter* interpreter, const std::vector<RuntimeValue>& arguments) = 0;
    virtual std::string toString() = 0;
};

#endif // CALLABLE_H
