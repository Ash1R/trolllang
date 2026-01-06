#include "../include/RuntimeValue.h"
#include "../include/Callable.h"
#include "../include/TrollFunction.h"
#include "../include/TrollArray.h"
#include "../include/TrollInstance.h"
#include "../include/TrollModel.h"
#include <cmath>

std::string to_string(const RuntimeValue& value) {
    if (std::holds_alternative<std::monostate>(value)) return "nil";
    if (std::holds_alternative<double>(value)) {
        double d = std::get<double>(value);
        // Remove trailing zeros logic could go here
        std::string s = std::to_string(d);
        return s;
    }
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
    if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
    if (std::holds_alternative<std::shared_ptr<Callable>>(value)) {
        return std::get<std::shared_ptr<Callable>>(value)->toString();
    }
    if (std::holds_alternative<std::shared_ptr<TrollArray>>(value)) {
        auto arr = std::get<std::shared_ptr<TrollArray>>(value);
        std::string s = "[";
        for (size_t i = 0; i < arr->elements.size(); ++i) {
            s += to_string(arr->elements[i]);
            if (i < arr->elements.size() - 1) s += ", ";
        }
        s += "]";
        return s;
    }
    if (std::holds_alternative<std::shared_ptr<TrollInstance>>(value)) {
        auto inst = std::get<std::shared_ptr<TrollInstance>>(value);
        return "instance of " + inst->model->name;
    }
    return "";
}
