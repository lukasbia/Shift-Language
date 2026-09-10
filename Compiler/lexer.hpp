#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <regex>

struct SourceLocation {
    size_t line;
    size_t column;
};

struct Token {
    std::string type;
    std::string value;
    SourceLocation loc;
};

class Lexer {
private:
    std::string code;
    size_t pos = 0;
    size_t line = 1;
    size_t col = 1;
    std::stack<int> indentStack;

public:
    Lexer(const std::string& sourceCode);
    std::vector<Token> tokenize();
};

#endif
