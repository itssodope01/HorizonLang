#include "Lexer.h"
#include <iostream>

std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"elif", TokenType::ELIF},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"try", TokenType::TRY},
    {"catch", TokenType::CATCH},
    {"return", TokenType::RETURN},
    {"const", TokenType::CONST},
    {"fx", TokenType::FX},
    {"int", TokenType::INT},
    {"float", TokenType::FLOAT},
    {"string", TokenType::STRING},
    {"bool", TokenType::BOOL},
    {"void", TokenType::VOID},
    {"list", TokenType::LIST},
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT},
    {"true", TokenType::BOOL_LITERAL},
    {"false", TokenType::BOOL_LITERAL},
    {"print", TokenType::PRINT},
    {"input", TokenType::INPUT}
};

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        start = current;
        tokenColumn = column;

        skipWhitespace();
        if (isAtEnd()) break;

        start = current;
        tokenColumn = column;

        char c = advance();

        if (isDigit(c)) {
            tokens.push_back(handleNumber());
            continue;
        }

        if (isAlpha(c)) {
            tokens.push_back(handleIdentifier());
            continue;
        }

        switch (c) {
            case '(': tokens.push_back(makeToken(TokenType::LPAREN)); break;
            case ')': tokens.push_back(makeToken(TokenType::RPAREN)); break;
            case '{': tokens.push_back(makeToken(TokenType::LBRACE)); break;
            case '}': tokens.push_back(makeToken(TokenType::RBRACE)); break;
            case '[': tokens.push_back(makeToken(TokenType::LBRACKET)); break;
            case ']': tokens.push_back(makeToken(TokenType::RBRACKET)); break;
            case ',': tokens.push_back(makeToken(TokenType::COMMA)); break;
            case '.': tokens.push_back(makeToken(TokenType::DOT)); break;
            case ';': tokens.push_back(makeToken(TokenType::SEMICOLON)); break;
            case '+': tokens.push_back(makeToken(TokenType::PLUS)); break;
            case '-': tokens.push_back(makeToken(TokenType::MINUS)); break;
            case '*': tokens.push_back(makeToken(TokenType::MULTIPLY)); break;
            case '/':
                if (match('*')) {
                    skipMultiLineComment();
                } else {
                    tokens.push_back(makeToken(TokenType::DIVIDE));
                }
                break;
            case '#':
                // Handle comments starting with '#'
                skipSingleLineComment();
                break;
            case '%': tokens.push_back(makeToken(TokenType::MODULO)); break;
            case '=':
                tokens.push_back(makeToken(match('=') ? TokenType::EQUALS : TokenType::ASSIGN));
                break;
            case '!':
                tokens.push_back(makeToken(match('=') ? TokenType::NOT_EQUALS : TokenType::NOT));
                break;
            case '<':
                tokens.push_back(makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS_THAN));
                break;
            case '>':
                tokens.push_back(makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER_THAN));
                break;
            case '"': tokens.push_back(handleString()); break;
            default:
                std::cerr << "Error at line " << line << ", column " << (column - 1)
                          << ": Unexpected character '" << c << "'." << std::endl;
                break;
        }
    }

    tokens.emplace_back(Token(TokenType::END_OF_FILE, "", line, column));
    return tokens;
}

// Helper methods
bool Lexer::isAtEnd() const {
    return current >= source.length();
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';

    char c = source[current];
    current++;

    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }

    return c;
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    column++;
    return true;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                advance();
                break;
            default:
                return;
        }
    }
}

void Lexer::skipSingleLineComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

void Lexer::skipMultiLineComment() {
    while (!isAtEnd()) {
        if (peek() == '*' && peekNext() == '/') {
            advance(); // Consume '*'
            advance(); // Consume '/'
            break;
        } else {
            if (peek() == '\n') {
                line++;
                column = 1;
            }
            advance();
        }
    }
}

Token Lexer::handleNumber() {
    while (isDigit(peek())) advance();

    // Look for a fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // Consume '.'

        while (isDigit(peek())) advance();
        return makeToken(TokenType::FLOAT_LITERAL);
    }

    return makeToken(TokenType::INTEGER_LITERAL);
}

Token Lexer::handleString() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "Error at line " << line << ", column " << column
                  << ": Unterminated string." << std::endl;
        return makeToken(TokenType::INVALID); // Unterminated string
    }

    advance();
    return makeToken(TokenType::STRING_LITERAL);
}

Token Lexer::handleIdentifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords[text] : TokenType::IDENTIFIER;
    return makeToken(type);
}

Token Lexer::makeToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    Token token(type, text, line, tokenColumn);
    return token;
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}
