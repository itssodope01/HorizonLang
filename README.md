# Horizon Lang

Horizon Lang is a custom programming language designed as a learning project to understand the fundamentals of compiler design, including lexing, parsing, semantic analysis, and code generation. The language supports high-level constructs, static typing, and a simplified syntax that allows for expressive programming. It can compile to both Python and C++.

This repository provides all the necessary components to build, extend, and experiment with a custom programming language.

---

## Features

- **Static typing**: Supports `int`, `float`, `string`, `bool`, `list`, and `void` types.
- **Control structures**: Includes `if`, `else`, `elseif`, `while`, and `for` statements.
- **Functions**: Define reusable functions with `fx` keyword.
- **Error handling**: Supports `try`-`catch` blocks for exception management.
- **Built-in I/O**: Use `print` and `input` for standard input/output operations.
- **Custom operators**: Logical (`and`, `or`, `not`), comparison (`==`, `!=`, `<`, `>`), and arithmetic (`+`, `-`, `*`, `/`, `%`) operators.
- **List operations**: Allows access, modification, and built-in methods for lists.
- **Multiple backends**: Compile to Python or C++ for flexibility and platform independence.

---

## Grammar

The full grammar for Horizon Lang can be found in the [GRAMMARS.md](./GRAMMARS.md) file. It includes detailed syntax definitions for statements, expressions, and type systems.

---

## Examples

Examples of Horizon Lang programs are available in the [examples](./examples) folder. These examples demonstrate:

- Variable declarations
- Control flow statements
- Function definitions and calls
- List manipulations
- Error handling with `try`-`catch`

Example `hello.hl`:

```horizon
fx hello ( string name ) {
    print("Hello, " + name + " !");
}

string name = input("Enter your name: ");
hello(name);
```

---

## Getting Started

### Prerequisites

- **C++ compiler**: Ensure a modern C++ compiler (e.g., GCC, Clang) is installed.
- **Python 3**: Required for Python code compilation and execution.
- **CMake**: For building the C++ backend.
- **Git**: To clone the repository.

### Building the Compiler

Clone the repository and navigate to its directory:

```bash
git clone https://github.com/itssodope01/HorizonLang.git
cd HorizonLang
```

Build the project using `cmake`:

```bash
mkdir build && cd build
cmake ..
make
```

---

## Running Horizon Lang Programs

### Steps to Compile and Run

1. Create a `.hl` file with your Horizon Lang code, or use examples from the `examples` folder.
2. Run the compiler with your file as an argument:

   ```bash
   ./HorizonLang examples/hello.hl
   ```

3. Choose your target compiler when prompted:
    - `1` for Python
    - `2` for C++

4. If you choose Python, the compiler will generate and run `output.py`. For C++, it will build and execute a binary using the generated `output.cpp`.

---

## Example Workflow

Here’s an example of compiling and running a Horizon Lang program:

1. Create a program `examples/example.hl`:

   ```horizon
   fx main() -> void {
       int x = 10;
       print("Value of x: " + STR(x));
   }
   ```

2. Run the compiler:

   ```bash
   ./HorizonLang examples/example.hl
   ```

3. Choose `1` to generate and run Python code. The output will be:

   ```plaintext
   Value of x: 10
   ```

---

## Directory Structure

- **`lexer/`**: Handles lexical analysis, breaking the input into tokens.
- **`parser/`**: Parses tokens into an Abstract Syntax Tree (AST).
- **`ast/`**: AST structure and printing.
- **`semantic/`**: Performs semantic analysis, ensuring type and scope correctness.
- **`codegen/`**: Generates code for target backends (Python and C++).
- **`examples/`**: Sample Horizon Lang programs.
- **`GRAMMARS.md`**: Language grammar documentation.
- **`main.cpp`**: Entry point for the compiler.

---

## Contributing

Contributions are welcome! If you’d like to add features, fix bugs, or improve documentation, please:

1. Fork the repository.
2. Create a feature branch.
3. Submit a pull request.

---

## Acknowledgments

This project was inspired by the desire to explore compiler design and the intricacies of building a custom language. It serves as a stepping stone for anyone interested in programming language theory or implementation.

---

## License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.