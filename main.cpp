#include "lexer/Lexer.h"
#include "lexer/Token.h"
#include "lexer/Token_Utils.h"
#include <iostream>

void printToken(const Token& token) {
    std::cout << "Line " << token.line << ", Column " << token.column
              << ": Type=" << tokenTypeToString(token.type)
              << ", Lexeme='" << token.lexeme << "'" << std::endl;
}

int main() {
    // Test input based on your language syntax
    std::string testInput = R"(
        # This is a test program
        fx main() {
            const int x = 42;
            float y = 3.14;
            string msg = "Hello, World!";
            
            for (x, 1, 5) {
                print(msg);
            }
        }
    )";

    Lexer lexer(testInput);
    auto tokens = lexer.tokenize();

    // Print all tokens
    for (const auto& token : tokens) {
        printToken(token);
    }

    return 0;
}
