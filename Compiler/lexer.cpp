#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string& source) : src(source), pos(0) {}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return src[pos];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    return src[pos++];
}

bool Lexer::isAtEnd() {
    return pos >= src.length();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        char c = peek();

        // Skip whitespace
        if (std::isspace(c)) {
            advance();
            continue;
        }

        // Parse hardware-specific attributes (e.g. .volatile, .address)
        if (c == '.') {
            advance();
            std::string attr = ".";
            while (std::isalnum(peek()) || peek() == '_') {
                attr += advance();
            }
            tokens.push_back({TokenType::TOKEN_ATTRIBUTE, attr});
            continue;
        }

        // Parse Identifiers and Keywords
        if (std::isalpha(c) || c == '_') {
            std::string ident = "";
            while (std::isalnum(peek()) || peek() == '_') {
                ident += advance();
            }
            
            TokenType type = TokenType::TOKEN_IDENTIFIER;
            if (ident == "var" || ident == "let" || ident == "func" || ident == "if" || ident == "while") {
                type = TokenType::TOKEN_KEYWORD;
            }
            
            tokens.push_back({type, ident});
            continue;
        }

        // Parse Numeric Literals (Decimals and Hexadecimals)
        if (std::isdigit(c)) {
            std::string num = "";
            while (std::isalnum(peek()) || peek() == '.') { // handles 0x hex values too
                num += advance();
            }
            tokens.push_back({TokenType::TOKEN_NUMBER, num});
            continue;
        }

        // Parse Symbols
        if (c == '=' || c == ':' || c == '+' || c == '-' || c == '(' || c == ')' || c == '{' || c == '}') {
            std::string sym(1, advance());
            tokens.push_back({TokenType::TOKEN_SYMBOL, sym});
            continue;
        }

        // Skip unrecognized single characters safely
        advance();
    }

    tokens.push_back({TokenType::TOKEN_EOF, ""});
    return tokens;
}
