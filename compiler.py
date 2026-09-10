import sys
import os
from compiler.parser import parse

def main():
    if len(sys.argv) < 2:
        print("Usage: python -m compiler.shiftc <file.shift | file.shiftui>")
        sys.exit(1)

    filepath = sys.argv[1]
    if not os.path.exists(filepath):
        print(f"Error: File '{filepath}' not found.")
        sys.exit(1)

    with open(filepath, "r") as f:
        code = f.read()

    ext = os.path.splitext(filepath)[1]
    print(f"[{ext}] Processing file through shiftc pipeline...")

    try:
        tokens = parse(code)
        if ext == ".shiftui":
            print("Target: WebAssembly (WASM) -> Ready for sjsb binding.")
        elif ext == ".shift":
            print("Target: LLVM IR -> Bare Metal Machine Code.")
        else:
            print(f"Unrecognized file extension: '{ext}'")
    except SyntaxError as e:
        print(f"\nCompilation Failed:\n{e}")

if __name__ == "__main__":
    main()
