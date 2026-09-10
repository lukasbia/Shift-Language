from compiler.lexer import tokenize

# --- AST Node Definitions ---
class ProgramNode:
    def __init__(self, statements):
        self.statements = statements

class FunctionNode:
    def __init__(self, name, body):
        self.name = name
        self.body = body

class VarDeclNode:
    def __init__(self, name, attributes, value):
        self.name = name
        self.attributes = attributes
        self.value = value

class AttributeNode:
    def __init__(self, name, args):
        self.name = name
        self.args = args

# --- Indentation-Aware Parser ---
class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.pos = 0

    def peek(self, offset=0):
        idx = self.pos + offset
        return self.tokens[idx] if idx < len(self.tokens) else (None, None)

    def consume(self, expected_type=None):
        token_type, value = self.peek()
        if expected_type and token_type != expected_type:
            raise SyntaxError(f"Expected {expected_type}, got '{value}' ({token_type})")
        self.pos += 1
        return token_type, value

    def parse_attribute(self):
        _, attr_name = self.consume("attribute")
        args = []
        if self.peek()[0] == "leftParen":
            self.consume("leftParen")
            _, arg_val = self.consume()
            args.append(arg_val)
            self.consume("rightParen")
        return AttributeNode(attr_name, args)

    def parse_statement(self):
        attributes = []

        # Collect leading attributes (.volatile, .address)
        while self.peek()[0] == "attribute":
            attributes.append(self.parse_attribute())

        token_type, text = self.peek()

        # Variable Declarations: var x = 10
        if token_type == "keyword" and text in ("var", "let"):
            self.consume("keyword")
            _, var_name = self.consume("identifier")
            self.consume("assignOp")
            _, val = self.consume()
            return VarDeclNode(var_name, attributes, val)

        # Function Definitions: func executeControl():
        if token_type == "keyword" and text == "func":
            self.consume("keyword")
            _, func_name = self.consume("identifier")
            self.consume("leftParen")
            self.consume("rightParen")
            self.consume("colon")  # Expect Python-style colon ending

            body = []
            # Parse indented child statements
            while self.pos < len(self.tokens) and self.peek()[0] != "keyword":
                body.append(self.parse_statement())
            return FunctionNode(func_name, body)

        raise SyntaxError(f"Unexpected token '{text}' at index {self.pos}")

    def parse_program(self):
        statements = []
        while self.pos < len(self.tokens):
            statements.append(self.parse_statement())
        return ProgramNode(statements)

def parse(code):
    tokens = tokenize(code)
    parser = Parser(tokens)
    return parser.parse_program()
