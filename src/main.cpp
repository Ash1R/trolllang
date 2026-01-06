#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/AST.h"
#include "../include/Interpreter.h"
#include "../include/CodeGenerator.h"
#include <cstring>

// AST Printer was here, now switching to Interpreter Execution

void run(std::string source, bool compile) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    if (compile) {
        CodeGenerator codegen;
        codegen.generateCode(statements);
        codegen.saveModule("output.ll");
        std::cout << "Compiled to output.ll" << std::endl;
    } else {
        Interpreter interpreter;
        interpreter.interpret(statements);
    }
}

void runFile(const char* path, bool compile) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << path << std::endl;
        exit(74);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str(), compile);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: trolllang [-c] <script>" << std::endl;
        return 64;
    }
    
    bool compile = false;
    const char* file = nullptr;

    if (argc == 3 && strcmp(argv[1], "-c") == 0) {
        compile = true;
        file = argv[2];
    } else if (argc == 2) {
        file = argv[1];
    } else {
         std::cout << "Usage: trolllang [-c] <script>" << std::endl;
         return 64;
    }
    
    runFile(file, compile);
    return 0;
}
