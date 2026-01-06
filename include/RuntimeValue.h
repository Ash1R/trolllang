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

// We need to verify if Callable definition is available for to_string or handled separately.
// Since we only hold shared_ptr, it's fine. But to_string impl needs Callable definition if we use it here.
// We can move to_string impl to a cpp file or header that includes Callable.h, but circular include!
// Better to make to_string a declaration here and impl in Interpreter.cpp or RuntimeValue.cpp?
// For now, let's keep to_string inline but check types carefully.
// If we include Callable.h here, we get circular dep if Callable includes RuntimeValue.
// Callable.h includes RuntimeValue.h. So RuntimeValue.h CANNOT include Callable.h.
// So we must forward declare Callable.
// But we cannot call toString() on undefined type.
// So we must move to_string implementation out of inline header OR include Callable.h inside the function or similar hacks.
// Best practice: Move to_string logic to a .cpp file or make it a separate utility. 
// OR: Include Callable.h IN THE CPP files that use RuntimeValue, and here just use forward decl. 
// But to_string is used in Interpreter.cpp. 
// Let's remove inline definition of to_string from header and declare it.

std::string to_string(const RuntimeValue& value);


#endif // RUNTIME_VALUE_H
