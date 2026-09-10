import re

# 1. Complete set of 37 Reserved Keywords for Shift
KEYWORDS = {
    # Declarations & Scope
    "var", "let", "func", "asm", "type", "enum",
    # Memory & Storage Modifiers
    "mutating", "volatile", "static", "private", "public", "const", "atomic", "extern",
    # Control Flow
    "if", "else", "switch", "case", "for", "while", "return", "break", "continue",
    # Hardware & Systems Operations
    "align", "cast", "sizeOf", "defer", "unsafe",
    # Literals & Directives
    "true", "false", "nil", "import", "ui", "struct", "async", "await", "guard"
}

# 2. Token definitions (Strict Regex Order)
TOKEN_TYPES = [
    # Comments (Skipped or preserved)
    ("singleLineComment", r"//.*"),
    ("multiLineComment", r"/\*[\s\S]*?\*/"),

    # Attributes (.name, .type, .address, .volatile)
    ("attribute", r"\.[a-zA-Z_][a-zA-Z0-9_]*"),

    # Identifiers & Keywords (Handled dynamically in tokenize logic)
    ("identifier", r"[a-zA-Z_][a-zA-Z0-9_]*"),

    # Literals (hexLiteral MUST COME BEFORE numberLiteral)
    ("hexLiteral", r"0x[0-9a-fA-F]+"),
    ("numberLiteral", r"\d+(\.\d+)?"),
    ("stringLiteral", r'"[^"]*"'),

    # Comparison & Logical Operators
    ("equalOp", r"=="),
    ("notEqualOp", r"!="),
    ("lessThanOrEqualOp", r"<="),
    ("greaterThanOrEqualOp", r">="),
    ("logicalAndOp", r"&&"),
    ("logicalOrOp", r"\|\|"),

    # Arithmetic & Bitwise Operators
    ("assignOp", r"="),
    ("addOp", r"\+"),
    ("subOp", r"-"),
    ("mulOp", r"\*"),
    ("divOp", r"/"),
    ("bitwiseAndOp", r"&"),
    ("bitwiseOrOp", r"\|"),
    ("bitwiseXorOp", r"\^"),

    # Delimiters & Symbols
    ("semicolon", r";"),
    ("colon", r":"),
    ("comma", r","),
    ("leftBrace", r"\{"),
    ("rightBrace", r"\}"),
    ("leftParen", r"\("),
    ("rightParen", r"\)"),

    # Whitespace (Skipped by Lexer)
    ("whiteSpace", r"[ \t\n\r]+"),
]

def tokenize(code):
    pos = 0
    tokens = []
    
    while pos < len(code):
        match = None
        for token_type, regex in TOKEN_TYPES:
            pattern = re.compile(regex)
            match = pattern.match(code, pos)
            
            if match:
                text = match.group(0)
                
                # Skip comments and whitespace
                if token_type not in ("whiteSpace", "singleLineComment", "multiLineComment"):
                    if token_type == "identifier" and text in KEYWORDS:
                        tokens.append(("keyword", text))
                    else:
                        tokens.append((token_type, text))
                        
                pos = match.end()
                break
                
        if not match:
            raise SyntaxError(f"Unexpected character '{code[pos]}' at position {pos}")
            
    return tokens
