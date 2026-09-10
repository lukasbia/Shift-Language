#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>
#include <stdexcept>
#include <stack>

struct SourceLocation {
    size_t line;
    size_t column;
};

struct Token {
    std::string type;
    std::string value;
    SourceLocation loc;
};

// Complete set of 37 Reserved Keywords for Shift
const std::unordered_set<std::string> KEYWORDS = {
    // Declarations & Scope
    "var", "let", "func", "asm", "type", "enum",
    // Memory & Storage Modifiers
    "mutating", "volatile", "static", "private", "public", "const", "atomic", "extern",
    // Control Flow
    "if", "else", "switch", "case", "for", "while", "return", "break", "continue",
    // Hardware & Systems Operations
    "align", "cast", "sizeOf", "defer", "unsafe",
    // Literals & Directives
    "true", "false", "nil", "import", "ui", "struct", "async", "await", "guard"
};

struct TokenRule {
    std::string type;
    std::regex pattern;
};

class Lexer {
private:
    std::string code;
    size_t pos = 0;
    size_t line = 1;
    size_t col = 1;
    std::stack<int> indentStack;

public:
    Lexer(const std::string& sourceCode) : code(sourceCode) {
        indentStack.push(0); // Base indentation level
    }

    std::vector<Token> tokenize() {
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
            // 1. Handle Newlines and Indentation Tracking (Off-side rule)
            if (code[pos] == '\n' || code[pos] == '\r') {
                bool isCrLf = (code[pos] == '\r' && pos + 1 < length && code[pos + 1] == '\n');
                pos += isCrLf ? 2 : 1;
                line++;
                col = 1;
                
                tokens.push_back({"newline", "\n", {line, col}});

                // Measure indentation of the new line
                int indentLength = 0;
                while (pos < length && (code[pos] == ' ' || code[pos] == '\t')) {
                    indentLength += (code[pos] == '\t') ? 4 : 1; // Treat tab as 4 spaces
                    pos++;
                    col++;
                }

                // Ignore empty lines or pure comment lines for indentation stacking
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

            // 2. Skip inline horizontal whitespace
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

                    // Skip comments
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

        // Close any remaining open indents at EOF
        while (indentStack.size() > 1) {
            indentStack.pop();
            tokens.push_back({"dedent", "DEDENT", {line, col}});
        }

        tokens.push_back({"eof", "", {line, col}});
        return tokens;
    }
};
