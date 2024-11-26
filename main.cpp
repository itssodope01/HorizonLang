#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <limits>
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "lexer/Token_Utils.h"
#include "lexer/Token.h"
#include "ast/ASTprint.h"
#include "codegen/Python/PythonCodegen.h"
#include "codegen/CPP/CppCodeGen.h"
#include "semantic/SemanticAnalyzer.h"

enum class CompilerChoice {
    Python,
    Cpp,
    Invalid
};

CompilerChoice getCompilerChoice() {
    while (true) {
        std::cout << "\nPlease choose your target compiler:\n"
                  << "1. Python\n"
                  << "2. C++\n"
                  << "Enter choice (1 or 2): ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter 1 or 2.\n";
            continue;
        }

        switch (choice) {
            case 1: return CompilerChoice::Python;
            case 2: return CompilerChoice::Cpp;
            default:
                std::cout << "Invalid choice. Please enter 1 or 2.\n";
        }
    }
}

bool compilePython(const std::string& filename) {
    std::cout << "\nCompiling and running Python code...\n\n";
    int result = system(("python3 " + filename).c_str());
    return result == 0;
}

bool compileCpp(const std::string& filename) {
    std::cout << "\nCompiling C++ code...\n";

    // compilation command using CMake
    std::string buildCmd = "cmake --build . --target output_executable";
    if (system(buildCmd.c_str()) != 0) {
        std::cerr << "Failed to build C++ code using CMake\n";
        return false;
    }

    std::cout << "Running C++ executable...\n";
    int result = system("./output_executable");
    return result == 0;
}

void printToken(const Token& token) {
    std::cout << "Line " << token.line << ", Column " << token.column
              << ": Type=" << tokenTypeToString(token.type)
              << ", Lexeme='" << token.lexeme << "'" << std::endl;
}

std::string loadFileContent(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

bool isHlFile(const std::string& filename) {
    return filename.size() >= 3 && filename.substr(filename.size() - 3) == ".hl";
}

int main(int argc, char* argv[]) {
    std::string testInput;

    if (argc > 1) {
        if (!isHlFile(argv[1])) {
            std::cerr << "Error: Only '.hl' files are allowed as input." << std::endl;
            return 1;
        }
        try {
            testInput = loadFileContent(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cout << "No input file provided. Enter your code line by line (type END to finish):\n";

        testInput.clear();
        std::string line;

        while (true) {
            std::getline(std::cin, line);
            if (line == "END") {
                break;
            }
            testInput += line + '\n';
        }
        std::cout << "Input collection complete.\n";
    }

    try {
        Lexer lexer(testInput);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto program = parser.parse();

        if (parser.hadError || program == nullptr) {
            std::cerr << "\nParsing failed due to errors." << std::endl;
            for (const auto& msg : parser.errorMessages) {
                std::cerr << msg << std::endl;
            }
            std::cerr << "Total parsing errors: " << parser.errorCount << std::endl;
            return 1;
        }

        SemanticAnalyzer analyzer;
        if (!analyzer.analyze(program)) {
            std::cerr << "\nSemantic analysis failed due to errors:" << std::endl;
            for (const auto& error : analyzer.getErrors()) {
                std::cerr << error << std::endl;
            }
            // ASTPrinter::printAST(program);
            return 1;
        }

        std::cout << "\nParsing and Semantic analysis successful!\n" << std::endl;

        // Uncomment lines below to see AST Structure
        // std::cout << "\nAST structure:" << std::endl;
        // ASTPrinter::printAST(program);

        // Get user's compiler choice
        CompilerChoice choice = getCompilerChoice();

        try {
            if (choice == CompilerChoice::Python) {
                PythonCodeGen pycodegen;
                std::string pythonCode = pycodegen.generate(program);
                std::string outputFile = "output.py";
                pycodegen.writeToFile(outputFile, pythonCode);
                if (!compilePython(outputFile)) {
                    std::cerr << "Python compilation/execution failed\n";
                    return 1;
                }
            } else {
                CppCodeGen cppcodegen;
                std::string cppCode = cppcodegen.generate(program);
                std::string outputFile = "output.cpp";
                cppcodegen.writeToFile(outputFile, cppCode);
                if (!compileCpp(outputFile)) {
                    std::cerr << "C++ compilation/execution failed\n";
                    return 1;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error during code generation: " << e.what() << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during compilation: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}