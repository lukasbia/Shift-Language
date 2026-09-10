#include "lexer.hpp"
#include <stdexcept>

const std::unordered_set<std::string> KEYWORDS = {
    "var", "let", "func", "asm", "type", "enum",
    "mutating", "volatile", "static", "private", "public", "const", "atomic", "extern",
    "if", "else", "switch", "case", "for", "while", "return", "break", "continue",
    "align", "cast", "sizeOf", "defer", "unsafe",
    "true", "false", "nil", "import", "ui", "struct", "async", "await", "guard"
};

struct TokenRule {
    std::string type;
    std::regex pattern;
};

Lexer::Lexer(const std::string& sourceCode) : code(sourceCode) {
    indentStack.push(0);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    const std::vector<TokenRule> TOKEN_TYPES = {
        {"singleLineComment", std::regex(R"(//.*)")},
        {"multiLineComment", std::regex(R"(/\*[\s\S]*?\*/)")},
        {"attribute", std::regex(R"(\.[a-zA-Z_][a-zA-Z0-9_]*)")},
        {"identifier", std::regex(R"([a-zA-Z_][a-zA-Z0-9_]*)")},
        {"hexLiteral", std::regex(R"(0x[0-9a-fA-F]+)")},
        {"numberLiteral", std::regex(R"(\d+(\.\d+)?)")},
        {"stringLiteral", std::regex(R"("[^"]*")")},
        {"equalOp", std::regex(R"(==)")},
        {"notEqualOp", std::regex(R"(!=)")},
        {"lessThanOrEqualOp", std::regex(R"(<=)")},
        {"greaterThanOrEqualOp", std::regex(R"(>=)")},
        {"logicalAndOp", std::regex(R"(&&)")},
        {"logicalOrOp", std::regex(R"(\|\|)")},
        {"assignOp", std::regex(R"(=)")},
        {"addOp", std::regex(R"(\+)")},
        {"subOp", std::regex(R"(-)")},
        {"mulOp", std::regex(R"(\*)")},
        {"divOp", std::regex(R"(/)")},
        {"bitwiseAndOp", std::regex(R"(&)")},
        {"bitwiseOrOp", std::regex(R"(\|)")},
        {"bitwiseXorOp", std::regex(R"(\^))")},
        {"semicolon", std::regex(R"(;)")},
        {"colon", std::regex(R"(:)")},
        {"comma", std::regex(R"(,)")},
        {"leftParen", std::regex(R"(\()")},
        {"rightParen", std::regex(R"(\))")}
    };

    size_t length = code.length();

    while (pos < length) {
        if (code[pos] == '\n' || code[pos] == '\r') {
            bool isCrLf = (code[pos] == '\r' && pos + 1 < length && code[pos + 1] == '\n');
            pos += isCrLf ? 2 : 1;
            line++;
            col = 1;
            
            tokens.push_back({"newline", "\n", {line, col}});

            int indentLength = 0;
            while (pos < length && (code[pos] == ' ' || code[pos] == '\t')) {
                indentLength += (code[pos] == '\t') ? 4 : 1;
                pos++;
                col++;
            }

            if (pos < length && code[pos] != '\n' && code[pos] != '\r' && code[pos] != '/') {
                if (indentLength > indentStack.top()) {
                    indentStack.push(indentLength);
                    tokens.push_back({"indent", "INDENT", {line, col}});
                } else {
                    while (indentLength < indentStack.top()) {
                        indentStack.pop();
                        tokens.push_back({"dedent", "DEDENT", {line, col}});
                    }
                }
            }
            continue;
        }

        if (code[pos] == ' ' || code[pos] == '\t') {
            pos++;
            col++;
            continue;
        }

        bool matched = false;
        SourceLocation currentLoc = {line, col};

        for (const auto& rule : TOKEN_TYPES) {
            std::smatch match;
            if (std::regex_search(code.cbegin() + pos, code.cend(), match, rule.pattern, std::regex_constants::match_continuous)) {
                std::string text = match.str(0);

                if (rule.type != "singleLineComment" && rule.type != "multiLineComment") {
                    if (rule.type == "identifier" && KEYWORDS.find(text) != KEYWORDS.end()) {
                        tokens.push_back({"keyword", text, currentLoc});
                    } else {
                        tokens.push_back({rule.type, text, currentLoc});
                    }
                }

                pos += text.length();
                col += text.length();
                matched = true;
                break;
            }
        }

        if (!matched) {
            throw std::runtime_error("SyntaxError: Unexpected character '" + std::string(1, code[pos]) + 
                                     "' at line " + std::to_string(line) + ", col " + std::to_string(col));
        }
    }

    while (indentStack.size() > 1) {
        indentStack.pop();
        tokens.push_back({"dedent", "DEDENT", {line, col}});
    }

    tokens.push_back({"eof", "", {line, col}});
    return tokens;
}
