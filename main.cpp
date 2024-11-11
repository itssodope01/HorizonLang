#include <iostream>
#include <memory>
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "lexer/Token_Utils.h"
#include "lexer/Token.h"
#include "ast/ASTprint.h"

void printToken(const Token& token) {
    std::cout << "Line " << token.line << ", Column " << token.column
              << ": Type=" << tokenTypeToString(token.type)
              << ", Lexeme='" << token.lexeme << "'" << std::endl;
}

int main() {
    std::string testInput = R"(# Error handling
int count = 2;
try {
int result = 10 / count; # Will throw error when count is 0
for (j, 10, 0, 2) { # Counts down by 2
print(j);
}
} catch (error) {
print("Division by zero error caught");
count = 1;})";

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

        // Print all tokens
        for (const auto& token : tokens) {
            printToken(token);
        }

        std::cout << "\nParsing successful! AST structure:" << std::endl;
        ASTPrinter::printAST(program);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}