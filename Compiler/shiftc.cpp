#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class CodeGenerator {
private:
    std::stringstream irStream;
    int tempCounter = 0;

    std::string newTemp() {
        return "%t" + std::to_string(tempCounter++);
    }

public:
    std::string generate(ProgramNode* program) {
        // Emit standard LLVM module header
        irStream << "; ModuleID = 'ShiftModule'\n";
        irStream << "source_filename = \"main.shift\"\n\n";

        // First pass: Global variables (like hardware registers)
        for (const auto& stmt : program->statements) {
            if (auto varNode = dynamic_cast<VarDeclNode*>(stmt.get())) {
                generateGlobalVar(varNode);
            }
        }

        irStream << "\n";

        // Second pass: Functions
        for (const auto& stmt : program->statements) {
            if (auto funcNode = dynamic_cast<FunctionNode*>(stmt.get())) {
                generateFunction(funcNode);
            }
        }

        return irStream.str();
    }

private:
    void generateGlobalVar(VarDeclNode* node) {
        // Check for hardware attributes like .volatile or .address
        bool isVolatile = false;
        std::string addressVal = "";

        for (const auto& attr : node->attributes) {
            if (attr.name == ".volatile") {
                isVolatile = true;
            } else if (attr.name == ".address" && !attr.args.empty()) {
                addressVal = attr.args[0];
            }
        }

        // Emit LLVM IR global variable syntax
        // Example: @controlReg = volatile global i32 16384
        irStream << "@" << node.name << " = ";
        if (isVolatile) {
            irStream << "volatile ";
        }
        
        // If an explicit hardware address is given, map it using LLVM sections or inttoptr alignments
        if (!addressVal.empty()) {
            irStream << "global i32 " << node.value << ", align 4 ; address: " << addressVal << "\n";
        } else {
            irStream << "global i32 " << node.value << ", align 4\n";
        }
    }

    void generateFunction(FunctionNode* node) {
        // Emit standard LLVM IR function signature
        // Example: define i32 @initHardware() { ... }
        irStream << "define i32 @" << node.name << "() {\n";
        irStream << "entry:\n";

        // Generate body statements inside the function block
        for (const auto& stmt : node->body) {
            if (auto varNode = dynamic_cast<VarDeclNode*>(stmt.get())) {
                // Local variable allocation inside function (alloca)
                std::string reg = newTemp();
                irStream << "    " << reg << " = alloca i32, align 4\n";
                irStream << "    store i32 " << varNode->value << ", i32* " << reg << ", align 4\n";
            }
        }

        // Default return for native functions
        irStream << "    ret i32 0\n";
        irStream << "}\n\n";
    }
};
