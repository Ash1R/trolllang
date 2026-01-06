#ifndef TROLL_ARRAY_H
#define TROLL_ARRAY_H

#include <vector>
#include <string>
#include "RuntimeValue.h"

class TrollArray {
public:
    std::vector<RuntimeValue> elements;

    TrollArray() {}
    TrollArray(std::vector<RuntimeValue> elements) : elements(std::move(elements)) {}

    // Helper to check if it's a matrix (2D)
    // For now, we assume simple arrays
};

#endif // TROLL_ARRAY_H
