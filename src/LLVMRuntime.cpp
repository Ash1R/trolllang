#include <vector>
#include <iostream>
#include <cstdlib>

extern "C" {
    // Check if we need to export these symbols explicitly for dynamic linking, 
    // but for static linking (linking .o files), this is fine.

    // Opaque pointer to std::vector<double>
    void* troll_create_array(int size) {
        // Initialize with 0.0
        auto* vec = new std::vector<double>(size, 0.0);
        return static_cast<void*>(vec);
    }

    void troll_array_set(void* arr, int index, double value) {
        if (!arr) {
             std::cerr << "Runtime Error: Null pointer access\n";
             exit(1);
        }
        auto* vec = static_cast<std::vector<double>*>(arr);
        if (index >= 0 && index < (int)vec->size()) {
            (*vec)[index] = value;
        } else {
            std::cerr << "Runtime Error: Index " << index << " out of bounds (size: " << vec->size() << ")\n";
            exit(1);
        }
    }

    double troll_array_get(void* arr, int index) {
        if (!arr) {
             std::cerr << "Runtime Error: Null pointer access\n";
             exit(1);
        }
        auto* vec = static_cast<std::vector<double>*>(arr);
        if (index >= 0 && index < (int)vec->size()) {
            return (*vec)[index];
        }
        std::cerr << "Runtime Error: Index " << index << " out of bounds (size: " << vec->size() << ")\n";
        exit(1);
        return 0.0;
    }
    
    int troll_array_size(void* arr) {
        if (!arr) return 0;
        auto* vec = static_cast<std::vector<double>*>(arr);
        return (int)vec->size();
    }
    
    // Debug print
    void troll_print_array(void* arr) {
         if (!arr) {
             std::cout << "[]";
             return;
         }
         auto* vec = static_cast<std::vector<double>*>(arr);
         std::cout << "[";
         for (size_t i = 0; i < vec->size(); ++i) {
             std::cout << (*vec)[i];
             if (i < vec->size() - 1) std::cout << ", ";
         }
         std::cout << "]\n";
    }

    void troll_print_value(int type, double num, void* ptr) {
        if (type == 0) { // Number
            std::cout << num << "\n";
        } else if (type == 1) { // Array
            troll_print_array(ptr);
        } else if (type == 2) { // Boolean
             std::cout << (num != 0.0 ? "true" : "false") << "\n";
        } else {
            std::cout << "<Unknown Type " << type << ">\n";
        }
    }
}
