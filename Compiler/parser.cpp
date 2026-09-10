#include "parser.hpp"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& t) : tokens(t) {}

Token Parser::peek(int offset) {
    size_t idx = pos + offset;
    if (idx < tokens.size()) return tokens[idx];
    return {"eof", "", {0, 0}};
}

Token Parser::consume(const std::string& expectedType) {
    Token token = peek();
    if (!expectedType.empty() && token.type != expectedType) {
        throw std::runtime_error("SyntaxError: Expected token type '" + expectedType + 
                                 "', but got '" + token.type + "' ('" + token.value + 
                                 "') at line " + std::to_string(token.loc.line));
    }
    pos++;
    return token;
}

void Parser::skipNewlines() {
    while (peek().type == "newline") {
        consume("newline");
    }
}

AttributeNode Parser::parseAttribute() {
    consume("attribute");
    std::string name = tokens[pos - 1].value;
    std::vector<std::string> args;

    if (peek().type == "leftParen") {
        consume("leftParen");
        Token argToken = consume();
        args.push_back(argToken.value);
        consume("rightParen");
    }
    return {name, args};
}

std::unique_ptr<VarDeclNode> Parser::parseVarDecl(const std::vector<AttributeNode>& attributes) {
    consume("keyword");
    std::string varName = consume("identifier").value;
    consume("assignOp");
    
    Token valToken = consume();
    auto varNode = std::make_unique<VarDeclNode>();
    varNode->name = varName;
    varNode->attributes = attributes;
    varNode->value = valToken.value;
    return varNode;
}

std::unique_ptr<FunctionNode> Parser::parseFunction() {
    consume("keyword");
    std::string funcName = consume("identifier").value;
    consume("leftParen");
    consume("rightParen");
    consume("colon");

    auto funcNode = std::make_unique<FunctionNode>();
    funcNode->name = funcName;

    skipNewlines();

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

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    
    while (peek().type != "eof") {
        skipNewlines();
        if (peek().type == "eof") break;
        program->statements.push_back(parseStatement());
        skipNewlines();
    }
    
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    std::vector<AttributeNode> attributes;

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
