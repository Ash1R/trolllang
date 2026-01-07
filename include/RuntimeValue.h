#ifndef RUNTIME_VALUE_H
#define RUNTIME_VALUE_H

#include <variant>
#include <string>
#include <vector>
#include <iostream>

// Forward declaration for recursive types (like Arrays of RuntimeValues) if needed later
// For now, simple types.

// Forward declare Callable and TrollArray and TrollInstance
class Callable;
class TrollArray;
class TrollInstance;

using RuntimeValue = std::variant<std::monostate, double, bool, std::string, std::shared_ptr<Callable>, std::shared_ptr<TrollArray>, std::shared_ptr<TrollInstance>>;

std::string to_string(const RuntimeValue& value);

std::string to_string(const RuntimeValue& value);


#endif // RUNTIME_VALUE_H
