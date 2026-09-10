#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include "lexer.hpp"

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

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek(int offset = 0);
    Token consume(const std::string& expectedType = "");
    void skipNewlines();
    AttributeNode parseAttribute();
    std::unique_ptr<VarDeclNode> parseVarDecl(const std::vector<AttributeNode>& attributes);
    std::unique_ptr<FunctionNode> parseFunction();

public:
    Parser(const std::vector<Token>& t);
    std::unique_ptr<ProgramNode> parseProgram();
    std::unique_ptr<ASTNode> parseStatement();
};

#endif
