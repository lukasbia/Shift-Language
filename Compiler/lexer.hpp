#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

enum class TokenType {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_KEYWORD,
    TOKEN_ATTRIBUTE, // Handles hardware qualifiers like .volatile
    TOKEN_SYMBOL,    // Handles =, :, +, -, etc.
    TOKEN_UNKNOWN
};

struct Token {
    TokenType type;
    std::string text;
};

class Lexer {
public:
    Lexer(const std::string& src);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos;
    char peek();
    char advance();
    bool isAtEnd();
};

#endif
