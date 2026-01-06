#ifndef RETURN_H
#define RETURN_H

#include <exception>
#include "RuntimeValue.h"

struct Return : public std::exception {
    RuntimeValue value;

    Return(RuntimeValue value) : value(std::move(value)) {}
};

#endif // RETURN_H
