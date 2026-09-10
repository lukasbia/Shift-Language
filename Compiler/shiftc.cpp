#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"

class CodeGenerator {
private:
    std::stringstream irStream;
    int tempCounter = 0;

    std::string newTemp() {
        return "%t" + std::to_string(tempCounter++);
    }

public:
    std::string generate(ProgramNode* program) {
        std::stringstream irStreamLocal;
        irStreamLocal << "; ModuleID = 'ShiftModule'\n";
        irStreamLocal << "source_filename = \"main.shift\"\n\n";

        for (const auto& stmt : program->statements) {
            if (auto varNode = dynamic_cast<VarDeclNode*>(stmt.get())) {
                generateGlobalVar(varNode, irStreamLocal);
            }
        }

        irStreamLocal << "\n";

        for (const auto& stmt : program->statements) {
            if (auto funcNode = dynamic_cast<FunctionNode*>(stmt.get())) {
                generateFunction(funcNode, irStreamLocal);
            }
        }

        return irStreamLocal.str();
    }

private:
    void generateGlobalVar(VarDeclNode* node, std::stringstream& stream) {
        bool isVolatile = false;
        std::string addressVal = "";

        for (const auto& attr : node->attributes) {
            if (attr.name == ".volatile") {
                isVolatile = true;
            } else if (attr.name == ".address" && !attr.args.empty()) {
                addressVal = attr.args[0];
            }
        }

        stream << "@" << node->name << " = ";
        if (isVolatile) {
            stream << "volatile ";
        }
        stream << "global i32 " << node->value << ", align 4\n";
    }

    void generateFunction(FunctionNode* node, std::stringstream& stream) {
        stream << "define i32 @" << node->name << "() {\n";
        stream << "entry:\n";

        for (const auto& stmt : node->body) {
            if (auto varNode = dynamic_cast<VarDeclNode*>(stmt.get())) {
                std::string reg = newTemp();
                stream << "    " << reg << " = alloca i32, align 4\n";
                stream << "    store i32 " << varNode->value << ", i32* " << reg << ", align 4\n";
            }
        }

        stream << "    ret i32 0\n";
        stream << "}\n\n";
    }
};

int main() {
    std::string shiftSource = 
        ".volatile\n"
        "var controlReg = 0x4000\n\n"
        "func initHardware():\n"
        "    var status = 1\n";

    try {
        std::cout << "[1/3] Lexing source code...\n";
        Lexer lexer(shiftSource);
        std::vector<Token> tokens = lexer.tokenize();

        std::cout << "[2/3] Parsing tokens into AST...\n";
        Parser parser(tokens);
        auto ast = parser.parseProgram();

        std::cout << "[3/3] Generating LLVM IR...\n";
        CodeGenerator codegen;
        std::string llvmIR = codegen.generate(ast.get());

        std::cout << "\n--- Generated LLVM IR Output ---\n";
        std::cout << llvmIR;

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << '\n';
    }

    return 0;
}
