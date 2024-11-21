#include "Parser.h"
#include <iostream>
#include <sstream>
#include "../lexer/Token.h"
#include "../ast/AST.h"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::shared_ptr<Program> Parser::parse() {
    std::vector<StmtPtr> statements;

    while (!isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (const ParseError& error) {
            hadError = true;
            errorCount++;
            std::stringstream ss;
            ss << "Parse error at line " << error.token.line << ", column "
               << error.token.column << ": " << error.what();
            errorMessages.push_back(ss.str());
            synchronize();
        }
    }

    if (hadError) {
        return nullptr;
    }

    return std::make_shared<Program>(std::move(statements));
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::FX:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
            case TokenType::TRY:
            case TokenType::CATCH:
            case TokenType::PRINT:
            case TokenType::INPUT:
                return;
            default:
                advance();
        }
    }
}

StmtPtr Parser::declaration() {
    if (match(TokenType::FX)) return functionDeclaration();

    size_t start = current;

    try {
        return varDeclaration();
    } catch (const ParseError& e) {
        current = start;
    }

    return statement();
}


bool Parser::isType() {
    return check(TokenType::INT) || check(TokenType::FLOAT) ||
           check(TokenType::STRING) || check(TokenType::BOOL) ||
           check(TokenType::LIST);
}

StmtPtr Parser::varDeclaration() {
    bool isConst = match(TokenType::CONST);
    TypePtr type = parseType();
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    ExprPtr initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<VarDecl>(isConst, type, name.lexeme, initializer);
}

StmtPtr Parser::functionDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    consume(TokenType::LPAREN, "Expect '(' after function name.");

    std::vector<std::pair<TypePtr, std::string>> parameters;
    if (!check(TokenType::RPAREN)) {
        do {
            TypePtr paramType = parseType();
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            parameters.emplace_back(paramType, paramName.lexeme);
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RPAREN, "Expect ')' after parameters.");
    consume(TokenType::LBRACE, "Expect '{' before function body.");

    std::vector<StmtPtr> body = block();
    return std::make_shared<Function>(name.lexeme, parameters, body);
}

StmtPtr Parser::statement() {
    if (match(TokenType::PRINT)) return printStatement();
    if (match(TokenType::INPUT)) return inputStatement();
    if (match(TokenType::IF)) return ifStatement();
    if (match(TokenType::WHILE)) return whileStatement();
    if (match(TokenType::FOR)) return forStatement();
    if (match(TokenType::TRY)) return tryCatchStatement();
    if (match(TokenType::RETURN)) return returnStatement();
    if (match(TokenType::ENDLOOP)) return EndLoopStatement();
    if (match(TokenType::NEXT)) return NextStatement();
    return expressionStatement();
}

StmtPtr Parser::expressionStatement() {
    ExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExpressionStatement>(expr);
}

StmtPtr Parser::tryCatchStatement() {
    consume(TokenType::LBRACE, "Expect '{' after 'try'.");
    std::vector<StmtPtr> tryBlock = block();

    consume(TokenType::CATCH, "Expect 'catch' after 'try' block.");
    consume(TokenType::LPAREN, "Expect '(' after 'catch'.");
    Token exceptionName = consume(TokenType::IDENTIFIER, "Expect exception variable name.");
    consume(TokenType::RPAREN, "Expect ')' after exception variable name.");
    consume(TokenType::LBRACE, "Expect '{' after 'catch(...)'.");

    std::vector<StmtPtr> catchBlock = block();

    return std::make_shared<TryCatch>(tryBlock, exceptionName.lexeme, catchBlock);
}

StmtPtr Parser::returnStatement() {
    ExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<Return>(value);
}

StmtPtr Parser::ifStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'if'.");
    ExprPtr condition = expression();
    consume(TokenType::RPAREN, "Expect ')' after if condition.");
    consume(TokenType::LBRACE, "Expect '{' before if block.");

    std::vector<StmtPtr> thenBlock = block();
    std::vector<std::pair<ExprPtr, std::vector<StmtPtr>>> elifBlocks;
    std::vector<StmtPtr> elseBlock;

    while (match(TokenType::ELSEIF)) {
        consume(TokenType::LPAREN, "Expect '(' after 'elif'.");
        ExprPtr elifCondition = expression();
        consume(TokenType::RPAREN, "Expect ')' after elif condition.");
        consume(TokenType::LBRACE, "Expect '{' before elif block.");
        std::vector<StmtPtr> elifBody = block();
        elifBlocks.emplace_back(elifCondition, elifBody);
    }

    if (match(TokenType::ELSE)) {
        consume(TokenType::LBRACE, "Expect '{' before else block.");
        elseBlock = block();
    }

    return std::make_shared<If>(condition, thenBlock, elifBlocks, elseBlock);
}

StmtPtr Parser::whileStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'while'.");
    ExprPtr condition = expression();
    consume(TokenType::RPAREN, "Expect ')' after while condition.");
    consume(TokenType::LBRACE, "Expect '{' before while block.");

    std::vector<StmtPtr> body = block();
    return std::make_shared<While>(condition, body);
}

StmtPtr Parser::forStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'for'.");
    Token iterator = consume(TokenType::IDENTIFIER, "Expect iterator variable.");
    consume(TokenType::COMMA, "Expect ',' after iterator.");

    ExprPtr start = expression();
    consume(TokenType::COMMA, "Expect ',' after start value.");
    ExprPtr end = expression();

    ExprPtr step = nullptr;
    if (match(TokenType::COMMA)) {
        step = expression();
    }

    consume(TokenType::RPAREN, "Expect ')' after for condition.");
    consume(TokenType::LBRACE, "Expect '{' before for block.");

    std::vector<StmtPtr> body = block();
    return std::make_shared<For>(iterator.lexeme, start, end, step, body);
}

StmtPtr Parser::printStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'print'.");
    ExprPtr value = expression();
    consume(TokenType::RPAREN, "Expect ')' after print value.");
    consume(TokenType::SEMICOLON, "Expect ';' after print statement.");
    return std::make_shared<Print>(value);
}

StmtPtr Parser::inputStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'input'.");
    ExprPtr prompt = expression();
    consume(TokenType::RPAREN, "Expect ')' after input prompt.");
    consume(TokenType::SEMICOLON, "Expect ';' after input statement.");
    return std::make_shared<Input>(prompt);
}

StmtPtr Parser::listOrStringOperation(const std::string& name) {
    Token op = consume(TokenType::IDENTIFIER, "Expect operation name.");
    consume(TokenType::LPAREN, "Expect '(' after operation name.");

    std::vector<ExprPtr> args;
    if (!check(TokenType::RPAREN)) {
        do {
            args.push_back(expression());
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RPAREN, "Expect ')' after arguments.");
    consume(TokenType::SEMICOLON, "Expect ';' after operation.");


    ExprPtr object = std::make_shared<Identifier>(name);
    ExprPtr callee = std::make_shared<MemberAccess>(object, op.lexeme);
    ExprPtr funcCall = std::make_shared<FunctionCall>(callee, args);


    return std::make_shared<ExpressionStatement>(funcCall);
}

StmtPtr Parser::assignmentStatement(const std::string& name) {
    ExprPtr target = std::make_shared<Identifier>(name);

    if (previous().type == TokenType::LBRACKET) {
        ExprPtr index = expression();
        consume(TokenType::RBRACKET, "Expect ']' after index.");
        target = std::make_shared<ListAccess>(target, index);
        consume(TokenType::ASSIGN, "Expect '=' after indexed variable.");
    }

    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after assignment.");

    // Create the Assignment expression
    ExprPtr assignmentExpr = std::make_shared<Assignment>(target, value);

    // Return an ExpressionStatement wrapping the Assignment
    return std::make_shared<ExpressionStatement>(assignmentExpr);
}

ExprPtr Parser::assignment() {
    ExprPtr expr = logicalOr();

    if (match(TokenType::ASSIGN)) {
        Token equals = previous();
        ExprPtr value = assignment();


        if (auto var = std::dynamic_pointer_cast<Identifier>(expr)) {
            return std::make_shared<Assignment>(expr, value);
        } else if (auto listAccess = std::dynamic_pointer_cast<ListAccess>(expr)) {
            return std::make_shared<Assignment>(expr, value);
        } else if (auto memberAccess = std::dynamic_pointer_cast<MemberAccess>(expr)) {
            return std::make_shared<Assignment>(expr, value);
        } else {
            throw ParseError("Invalid assignment target.", equals);
        }
    }

    return expr;
}

StmtPtr Parser::functionCallStatement(const std::string& name) {
    std::vector<ExprPtr> arguments;
    if (!check(TokenType::RPAREN)) {
        do {
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RPAREN, "Expect ')' after arguments.");
    consume(TokenType::SEMICOLON, "Expect ';' after function call.");

    ExprPtr funcCall = std::make_shared<FunctionCall>(std::make_shared<Identifier>(name), arguments);
    return std::make_shared<ExpressionStatement>(funcCall);
}

StmtPtr Parser::EndLoopStatement() {
    Token EndLoopToken = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'next'.");
    auto nextStmt = std::make_shared<ENDLOOP>();
    nextStmt->line = EndLoopToken.line;
    nextStmt->column = EndLoopToken.column;
    return nextStmt;
}

StmtPtr Parser::NextStatement() {
    Token skipitToken = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'next'.");
    auto skipitStmt = std::make_shared<NEXT>();
    skipitStmt->line = skipitToken.line;
    skipitStmt->column = skipitToken.column;
    return skipitStmt;
}


std::vector<StmtPtr> Parser::block() {
    std::vector<StmtPtr> statements;

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (const ParseError& error) {
            hadError = true;
            errorCount++;
            std::stringstream ss;
            ss << "Parse error at line " << error.token.line << ", column "
               << error.token.column << ": " << error.what();
            errorMessages.push_back(ss.str());
            synchronize();
        }
    }

    try {
        consume(TokenType::RBRACE, "Expect '}' after block.");
    } catch (const ParseError& error) {
        hadError = true;
        errorCount++;
        std::stringstream ss;
        ss << "Parse error at line " << error.token.line << ", column "
           << error.token.column << ": " << error.what();
        errorMessages.push_back(ss.str());
        synchronize();
    }
    return statements;
}

ExprPtr Parser::expression() {
    return assignment();
}

ExprPtr Parser::logicalOr() {
    ExprPtr expr = logicalAnd();

    while (match(TokenType::OR)) {
        ExprPtr right = logicalAnd();
        expr = std::make_shared<BinaryOp>(BinaryOp::Operator::OR, expr, right);
    }

    return expr;
}

ExprPtr Parser::logicalAnd() {
    ExprPtr expr = equality();

    while (match(TokenType::AND)) {
        ExprPtr right = equality();
        expr = std::make_shared<BinaryOp>(BinaryOp::Operator::AND, expr, right);
    }

    return expr;
}

ExprPtr Parser::equality() {
    ExprPtr expr = comparison();

    while (match(TokenType::EQUALS) || match(TokenType::NOT_EQUALS)) {
        Token op = previous();
        ExprPtr right = comparison();
        auto opType = op.type == TokenType::EQUALS ?
            BinaryOp::Operator::EQ : BinaryOp::Operator::NE;
        expr = std::make_shared<BinaryOp>(opType, expr, right);
    }

    return expr;
}

ExprPtr Parser::comparison() {
    ExprPtr expr = term();

    while (match(TokenType::GREATER_THAN) || match(TokenType::GREATER_EQUAL) ||
           match(TokenType::LESS_THAN) || match(TokenType::LESS_EQUAL)) {
        Token op = previous();
        ExprPtr right = term();

        BinaryOp::Operator opType;
        switch (op.type) {
            case TokenType::GREATER_THAN: opType = BinaryOp::Operator::GT; break;
            case TokenType::GREATER_EQUAL: opType = BinaryOp::Operator::GE; break;
            case TokenType::LESS_THAN: opType = BinaryOp::Operator::LT; break;
            case TokenType::LESS_EQUAL: opType = BinaryOp::Operator::LE; break;
            default: throw ParseError("Invalid comparison operator.", op);
        }

        expr = std::make_shared<BinaryOp>(opType, expr, right);
    }

    return expr;
}

ExprPtr Parser::term() {
    ExprPtr expr = factor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previous();
        ExprPtr right = factor();
        auto opType = op.type == TokenType::PLUS ?
            BinaryOp::Operator::ADD : BinaryOp::Operator::SUB;
        expr = std::make_shared<BinaryOp>(opType, expr, right);
    }

    return expr;
}

ExprPtr Parser::factor() {
    ExprPtr expr = unary();

    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULO)) {
        Token op = previous();
        ExprPtr right = unary();

        BinaryOp::Operator opType;
        switch (op.type) {
            case TokenType::MULTIPLY: opType = BinaryOp::Operator::MUL; break;
            case TokenType::DIVIDE: opType = BinaryOp::Operator::DIV; break;
            case TokenType::MODULO: opType = BinaryOp::Operator::MOD; break;
            default: throw ParseError("Invalid factor operator.", op);
        }

        expr = std::make_shared<BinaryOp>(opType, expr, right);
    }

    return expr;
}

ExprPtr Parser::unary() {
    if (match(TokenType::NOT) || match(TokenType::MINUS)) {
        Token op = previous();
        ExprPtr right = unary();
        auto opType = op.type == TokenType::NOT ?
            UnaryOp::Operator::NOT : UnaryOp::Operator::MINUS;
        return std::make_shared<UnaryOp>(opType, right);
    }

    return primary();
}

ExprPtr Parser::primary() {
    if (match(TokenType::BOOL_LITERAL)) {
        Token boolToken = previous();
        bool value = (boolToken.lexeme == "true");
        return std::make_shared<Literal>(value);
    }
    if (match(TokenType::INTEGER_LITERAL)) {
        Token num = previous();
        int value = std::stoi(num.lexeme);
        return std::make_shared<Literal>(value);
    }
    if (match(TokenType::FLOAT_LITERAL)) {
        Token num = previous();
        float value = std::stof(num.lexeme);
        return std::make_shared<Literal>(value);
    }
    if (match(TokenType::STRING_LITERAL)) {
        Token str = previous();
        return std::make_shared<Literal>(str.lexeme);
    }
    if (match(TokenType::LBRACKET)) { // Handle list literals
        std::vector<ExprPtr> elements;
        if (!check(TokenType::RBRACKET)) {
            do {
                elements.push_back(expression());
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RBRACKET, "Expect ']' after list elements.");
        return std::make_shared<Literal>(elements);
    }
    if (match(TokenType::INPUT)) {
        ExprPtr callee = std::make_shared<Identifier>("input");
        consume(TokenType::LPAREN, "Expect '(' after 'input'.");
        std::vector<ExprPtr> arguments;
        if (!check(TokenType::RPAREN)) {
            do {
                arguments.push_back(expression());
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RPAREN, "Expect ')' after arguments.");
        return std::make_shared<FunctionCall>(callee, arguments);
    }
    if (match(TokenType::IDENTIFIER)) {
        Token nameToken = previous();
        ExprPtr expr = std::make_shared<Identifier>(nameToken.lexeme);
        while (true) {
            if (match(TokenType::LPAREN)) {
                expr = functionCall(expr);
            } else if (match(TokenType::LBRACKET)) {
                ExprPtr index = expression();
                consume(TokenType::RBRACKET, "Expect ']' after index.");
                expr = std::make_shared<ListAccess>(expr, index);
            } else if (match(TokenType::DOT)) {
                Token memberName = consume(TokenType::IDENTIFIER, "Expect member name after '.'.");
                expr = std::make_shared<MemberAccess>(expr, memberName.lexeme);
            } else {
                break;
            }
        }
        return expr;
    }
    if (match(TokenType::LPAREN)) {
        ExprPtr expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return expr;
    }

    throw ParseError("Expect expression.", peek());
}

ExprPtr Parser::functionCall(ExprPtr callee) {
    std::vector<ExprPtr> arguments;
    if (!check(TokenType::RPAREN)) {
        do {
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RPAREN, "Expect ')' after arguments.");
    return std::make_shared<FunctionCall>(callee, arguments);
}

TypePtr Parser::parseType() {
    if (match(TokenType::INT)) return std::make_shared<Type>(Type::Kind::INT);
    if (match(TokenType::FLOAT)) return std::make_shared<Type>(Type::Kind::FLOAT);
    if (match(TokenType::STRING)) return std::make_shared<Type>(Type::Kind::STRING);
    if (match(TokenType::BOOL)) return std::make_shared<Type>(Type::Kind::BOOL);
    if (match(TokenType::LIST)) {
        consume(TokenType::LESS_THAN, "Expect '<' after 'list'.");
        TypePtr elementType = parseType();
        consume(TokenType::GREATER_THAN, "Expect '>' after list element type.");
        return std::make_shared<Type>(Type::Kind::LIST, elementType);
    }

    throw ParseError("Expect type.", peek());
}


// Utility methods
Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    if (current > 0) {
        throw ParseError(message, previous());
    } else {
        throw ParseError(message, peek());
    }
}

bool Parser::checkNext(TokenType type) const {
    if (current + 1 >= tokens.size()) return false;
    return tokens[current + 1].type == type;
}
