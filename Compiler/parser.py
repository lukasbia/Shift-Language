from compiler.lexer import tokenize

def validate_strict_rules(code):
    # Rule 1: Banned Operators
    banned_operators = ["+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "->"]
    for op in banned_operators:
        if op in code:
            raise SyntaxError(f"Shift Strict Rule #1 Violation: Operator '{op}' is banned. Use full explicit operations.")

    # Rule 2: Flat Ownership Check
    first_create = code.find("create.var")
    if first_create != -1:
        closing_colon = code.find(":", first_create)
        second_create = code.find("create.var", first_create + 10)
        if second_create != -1 and second_create < closing_colon:
            raise SyntaxError("Shift Strict Rule #2 Violation: Nested create blocks are strictly prohibited.")

def parse(code):
    validate_strict_rules(code)
    tokens = tokenize(code)
    print("Parsing successful: Code complies with all strict Shift language rules.")
    return tokens
