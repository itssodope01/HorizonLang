#pragma once
#include <string>

enum class TokenType {
    // Keywords
    IF,
    ELSE,
    ELIF,
    WHILE,
    FOR,
    TRY,
    CATCH,
    RETURN,
    CONST,
    FX,     // Function definition
    PRINT,
    INPUT,

    // Types
    INT,
    FLOAT,
    STRING,
    BOOL,
    LIST,

    // Literals
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL,

    // Operators
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    ASSIGN,         // =

    // Comparison
    EQUALS,         // ==
    NOT_EQUALS,     // !=
    LESS_THAN,      // <
    GREATER_THAN,   // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=

    // Logical
    AND,            // and
    OR,             // or
    NOT,            // not

    // Delimiters
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    LBRACKET,       // [
    RBRACKET,       // ]
    COMMA,          // ,
    SEMICOLON,      // ;
    DOT,            // .

    // Special
    IDENTIFIER,
    COMMENT,
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    // Constructor
    Token(TokenType t, std::string l, int ln, int col)
        : type(t), lexeme(std::move(l)), line(ln), column(col) {}
};
