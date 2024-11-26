
# Horizon Lang

Horizon Lang is a custom programming language designed as a learning project to understand the fundamentals of compiler design, including lexing, parsing, semantic analysis, and code generation. The language supports high-level constructs, static typing, and a simplified syntax that allows for expressive programming. It is Turing complete, meaning it can theoretically solve any computational problem or build large-scale applications, demonstrating the power and versatility of its design. It can compile to both Python and C++.

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

**Example `hello.hl`:**

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

#### Common Requirements

- **C++ compiler**: Ensure a modern C++ compiler (e.g., GCC, Clang) is installed.
- **Python 3**: Required for Python code compilation and execution.
- **CMake**: For building the C++ backend.
- **Git**: To clone the repository.

#### If you are using Windows

- **Windows Subsystem for Linux (WSL)**: Recommended for running Unix-like commands and environments on Windows.
   - **Installation Guide**: [Install WSL](https://docs.microsoft.com/en-us/windows/wsl/install)

> **⚠️ Important:**  
> Commands such as `cmake ..` and `make` **do not work** in native Windows Command Prompt or PowerShell. To execute these commands on Windows, you must use a Unix-like terminal provided by **WSL**.

---

### Additional Instructions for Windows Users

To ensure a smooth setup and build process on Windows, follow the detailed steps below:

#### 1. Install Windows Subsystem for Linux (WSL)

WSL allows you to run a Linux environment directly on Windows, which is essential for executing Unix-like commands such as `cmake` and `make`. Follow these steps to install WSL:

1. **Open PowerShell as Administrator**:
   - Press `Win + X` and select **Windows PowerShell (Admin)**.

2. **Enable WSL**:
    ```powershell
    wsl --install
    ```
   - This command installs the latest WSL version along with the default Linux distribution (usually Ubuntu).

3. **Restart Your Computer**:
   - After installation, you may be prompted to restart your machine.

4. **Set Up Your Linux Distribution**:
   - Upon restarting, launch the installed Linux distribution from the Start menu.
   - Follow the on-screen instructions to create a Unix username and password.

> **Alternative Installation Guide**: If you encounter issues, refer to the [official WSL installation guide](https://docs.microsoft.com/en-us/windows/wsl/install).

#### 2. Install Required Tools Within WSL (or in your Linux System if they are not already installed.)

Once WSL is set up, you'll need to install the necessary development tools inside the Linux environment:

1. **Update Package Lists**:
    ```bash
    sudo apt update
    ```

2. **Install Git, CMake, and a C++ Compiler**:
    ```bash
    sudo apt install git cmake build-essential
    ```

3. **Install Python 3**:
    ```bash
    sudo apt install python3
    ```

   - **Verify Installations**:
       ```bash
       git --version
       cmake --version
       g++ --version
       python3 --version
       ```

---

### Building the HorizonLang Compiler

> **Note for Windows Users:**  
> If you are on Windows, ensure you are executing the following commands within the **WSL** terminal.

1. **Clone the repository and navigate to its directory:**

    ```bash
    git clone https://github.com/itssodope01/HorizonLang.git
    cd HorizonLang
    ```

2. **Build the project using `cmake`:**

    ```bash
    mkdir build && cd build
    echo 'int main() {}' > output.cpp
    cmake ..
    make
    ```

---

## Running Horizon Lang Programs

### Steps to Compile and Run

> **Note:**  
> Ensure you are in the **WSL** terminal if you are on Windows.

1. **Create a `.hl` File**

   Create a `.hl` file with your HorizonLang code, or use examples from the `examples` folder.

2. **Run the Compiler**

   Run the compiler with your file as an argument (ensure you are in the `build` directory):

    ```bash
    ./HorizonLang ../examples/hello.hl
    ```

3. **Choose Your Target Compiler**

   When prompted, select your target compiler:
   - `1` for Python
   - `2` for C++

4. **Execute the Generated Code**

   - **Python**: The compiler will transpile and run `output.py`.
   - **C++**: The compiler will transpile `output.cpp`, build it, and execute the resulting binary.

5. **Optional: View the Transpiled Files**

   After compilation, you can view the generated Python or C++ files in two ways:

   - **Using the Terminal**:

     While in the `build` directory, use the `cat` command to display the contents of the generated file:

     ```bash
     cat output.py   # For Python
     ```

     ```bash
     cat output.cpp  # For C++
     ```

   - **Using a Code Editor**:

     Navigate to the `build` directory of the project in your code editor (e.g., VS Code, Sublime Text) to browse and inspect the generated `output.py` or `output.cpp` files visually.
     ```bash
     cd .. # Navigate to project root from build directory
     code . # opens the project in VS Code
     ```


6. **Optional: View the Abstract Syntax Tree (AST) Structure**

   To see the AST structure of your HorizonLang program:

   - **Uncomment AST Printing Lines**:

     Open `main.cpp` and uncomment lines 145 and 146 and save it:

     ```cpp
     // Uncomment lines below to see AST Structure
     // std::cout << "\nAST structure:" << std::endl;
     // ASTPrinter::printAST(program);
     ```

   - **Rebuild and Run**:

     Rebuild the project and run the compiler again to visualize the AST structure:

     ```bash
     make
     ./HorizonLang ../examples/hello.hl
     ```

     This will print the AST structure to the console, providing a detailed view of the parsed program.

---

## Example Workflow

Here’s an example of compiling and running a Horizon Lang program:

1. **Create a program `examples/example.hl`:**

    ```horizon
    fx main() {
        int x = 10;
        print("Value of x: " + STR(x));
    }
    ```

2. **Run the Compiler (make sure you are in the build directory):**

    ```bash
    ./HorizonLang ../examples/example.hl
    ```

3. **Choose `1` to generate and run Python code. The output will be:**

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

This project is licensed under the MIT License. See the [LICENSE](./LICENSE.md) file for details.