#pragma once
#include "../ast/AST.h"
#include "../lexer/Token.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, const Token& token)
        : std::runtime_error(message), token(token) {}

    Token token;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::shared_ptr<Program> parse();
    bool hadError = false;
    int errorCount = 0;
    std::vector<std::string> errorMessages;

private:
    std::vector<Token> tokens;
    size_t current = 0;

    // Utility methods
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool isAtEnd() const;
    Token consume(TokenType type, const std::string& message);
    void synchronize();
    bool isType();

    // Parsing methods
    StmtPtr declaration();
    StmtPtr varDeclaration();
    StmtPtr functionDeclaration();
    StmtPtr statement();

    StmtPtr expressionStatement();

    StmtPtr tryCatchStatement();

    std::vector<StmtPtr> block();
    StmtPtr ifStatement();
    StmtPtr whileStatement();
    StmtPtr forStatement();
    StmtPtr returnStatement();
    StmtPtr printStatement();
    StmtPtr inputStatement();
    StmtPtr listOrStringOperation(const std::string& name);
    StmtPtr assignmentStatement(const std::string& name);

    ExprPtr assignment();

    StmtPtr functionCallStatement(const std::string& name);

    ExprPtr expression();
    ExprPtr logicalOr();
    ExprPtr logicalAnd();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();
    ExprPtr functionCall(ExprPtr callee);

    TypePtr parseType();
};
