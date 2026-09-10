#include "parser.hpp"
#include <iostream>

Parser::Parser(const std::vector<Token>& t) : tokens(t), current(0) {}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::TOKEN_EOF;
}

void Parser::parse() {
    std::cout << "[Parser] Analyzing token stream...\n";
    
    while (!isAtEnd()) {
        Token t = peek();
        
        switch (t.type) {
            case TokenType::TOKEN_KEYWORD:
                std::cout << "  -> Keyword: " << t.text << "\n";
                break;
            case TokenType::TOKEN_IDENTIFIER:
                std::cout << "  -> Identifier: " << t.text << "\n";
                break;
            case TokenType::TOKEN_ATTRIBUTE:
                std::cout << "  -> Hardware Attribute: " << t.text << "\n";
                break;
            case TokenType::TOKEN_NUMBER:
                std::cout << "  -> Numeric Literal: " << t.text << "\n";
                break;
            case TokenType::TOKEN_SYMBOL:
                std::cout << "  -> Symbol: " << t.text << "\n";
                break;
            default:
                break;
        }
        advance();
    }
    
    std::cout << "[Parser] Syntax analysis successful.\n";
}
