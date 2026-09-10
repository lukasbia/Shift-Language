import re

TOKEN_TYPES = [
    ("CREATE_VAR", r"create\.var"),
    ("CREATE_ASM", r"create\.asm"),
    ("PROPERTY", r"\.[a-zA-Z_][a-zA-Z0-9_]*"),
    ("IDENTIFIER", r"[a-zA-Z_][a-zA-Z0-9_]*"),
    ("NUMBER", r"\d+"),
    ("HEX", r"0x[0-9a-fA-F]+"),
    ("ASSIGN", r"="),
    ("SEMICOLON", r";"),
    ("COLON", r":"),
    ("LBRACE", r"\{"),
    ("RBRACE", r"\}"),
    ("LPAREN", r"\("),
    ("RPAREN", r"\)"),
    ("STRING", r'"[^"]*"'),
    ("SKIP", r"[ \t\n]+"),
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
                if token_type != "SKIP":
                    tokens.append((token_type, text))
                pos = match.end()
                break
        if not match:
            raise SyntaxError(f"Lexer Error: Unrecognized token at position {pos}: '{code[pos]}'")
    return tokens
