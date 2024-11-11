#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string source;
    int current = 0;
    int start = 0;
    int line = 1;
    int column = 1;
    int tokenColumn = 1;

    static std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);

    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();
    Token handleNumber();
    Token handleString();
    Token handleIdentifier();
    Token makeToken(TokenType type);

    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
};
