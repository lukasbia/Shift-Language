#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

// --- AST Node Structures ---
struct ASTNode {
    virtual ~ASTNode() = default;
};

struct AttributeNode : public ASTNode {
    std::string name;
    std::vector<std::string> args;
};

struct VarDeclNode : public ASTNode {
    std::string name;
    std::vector<AttributeNode> attributes;
    std::string value;
};

struct FunctionNode : public ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> body;
};

struct ProgramNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
};

// --- C++ Parser Class ---
class Parser {
private:
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek(int offset = 0) {
        size_t idx = pos + offset;
        if (idx < tokens.size()) return tokens[idx];
        return {"eof", "", {0, 0}};
    }

    Token consume(const std::string& expectedType = "") {
        Token token = peek();
        if (!expectedType.empty() && token.type != expectedType) {
            throw std::runtime_error("SyntaxError: Expected token type '" + expectedType + 
                                     "', but got '" + token.type + "' ('" + token.value + 
                                     "') at line " + std::to_string(token.loc.line));
        }
        pos++;
        return token;
    }

    // Skip optional newlines
    void skipNewlines() {
        while (peek().type == "newline") {
            consume("newline");
        }
    }

    AttributeNode parseAttribute() {
        consume("attribute"); // e.g. .volatile or .address
        std::string name = tokens[pos - 1].value;
        std::vector<std::string> args;

        if (peek().type == "leftParen") {
            consume("leftParen");
            Token argToken = consume(); // Grab literal/identifier inside parenthesis
            args.push_back(argToken.value);
            consume("rightParen");
        }
        return {name, args};
    }

    std::unique_ptr<VarDeclNode> parseVarDecl(const std::vector<AttributeNode>& attributes) {
        consume("keyword"); // 'var' or 'let'
        std::string varName = consume("identifier").value;
        consume("assignOp");
        
        Token valToken = consume(); // Value (number, string, or identifier)
        auto varNode = std::make_unique<VarDeclNode>();
        varNode->name = varName;
        varNode->attributes = attributes;
        varNode->value = valToken.value;
        return varNode;
    }

    std::unique_ptr<FunctionNode> parseFunction() {
        consume("keyword"); // 'func'
        std::string funcName = consume("identifier").value;
        consume("leftParen");
        consume("rightParen");
        consume("colon");

        auto funcNode = std::make_unique<FunctionNode>();
        funcNode->name = funcName;

        skipNewlines();

        // Expect Python-style block indentation
        if (peek().type == "indent") {
            consume("indent");
            skipNewlines();

            while (peek().type != "dedent" && peek().type != "eof") {
                funcNode->body.push_back(parseStatement());
                skipNewlines();
            }

            if (peek().type == "dedent") {
                consume("dedent");
            }
        }

        return funcNode;
    }

public:
    Parser(const std::vector<Token>& t) : tokens(t) {}

    std::unique_ptr<ProgramNode> parseProgram() {
        auto program = std::make_unique<ProgramNode>();
        
        while (peek().type != "eof") {
            skipNewlines();
            if (peek().type == "eof") break;
            program->statements.push_back(parseStatement());
            skipNewlines();
        }
        
        return program;
    }

    std::unique_ptr<ASTNode> parseStatement() {
        std::vector<AttributeNode> attributes;

        // Collect leading attributes (e.g., .volatile var reg = 0x01)
        while (peek().type == "attribute") {
            attributes.push_back(parseAttribute());
        }

        Token token = peek();

        if (token.type == "keyword") {
            if (token.value == "var" || token.value == "let") {
                return parseVarDecl(attributes);
            }
            if (token.value == "func") {
                if (!attributes.empty()) {
                    throw std::runtime_error("SyntaxError: Attributes cannot be applied directly to functions.");
                }
                return parseFunction();
            }
        }

        throw std::runtime_error("SyntaxError: Unexpected token '" + token.value + 
                                 "' at line " + std::to_string(token.loc.line));
    }
};
