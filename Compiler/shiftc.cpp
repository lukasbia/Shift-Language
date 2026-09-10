#include <iostream>
#include <vector>
#include "lexer.hpp"
#include "parser.hpp"

int main() {
    std::cout << "=== Shift Native Compiler (shiftc) ===\n";

    // Testing a hardware-level declaration line in Shift
    std::string sampleCode = "var controlReg .volatile = 0x4000";
    
    std::cout << "Source Code:\n  " << sampleCode << "\n\n";

    // 1. Lexing Phase
    Lexer lexer(sampleCode);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Parsing Phase
    Parser parser(tokens);
    parser.parse();

    return 0;
}
