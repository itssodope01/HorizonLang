#pragma once
#include <vector>
#include <string>
#include <memory>
#include <variant>

// Forward declarations
class Expression;
class Statement;
class Type;

using ExprPtr = std::shared_ptr<Expression>;
using StmtPtr = std::shared_ptr<Statement>;
using TypePtr = std::shared_ptr<Type>;

// Base AST Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    int line;
    int column;
};

// Type System
class Type : public ASTNode {
public:
    enum class Kind {
        INT,
        FLOAT,
        STRING,
        BOOL,
        LIST,
        VOID,
        MATHOBJECT
    };

    Kind kind;
    TypePtr elementType;  // For List types

    explicit Type(Kind k, TypePtr elemType = nullptr)
        : kind(k), elementType(std::move(elemType)) {}
};

// Expression Nodes
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
    TypePtr type;
};

class Literal : public Expression {
public:
    std::variant<int, float, std::string, bool, std::vector<ExprPtr>> value;

    // Constructors for different literal types
    explicit Literal(int v) { value = v; }
    explicit Literal(float v) { value = v; }
    explicit Literal(const std::string& v) { value = v; }
    explicit Literal(bool v) { value = v; }
    explicit Literal(std::vector<ExprPtr> elements) { value = std::move(elements); }
};

class Identifier : public Expression {
public:
    std::string name;

    Identifier(std::string n)
        : name(std::move(n)) {}
};

class BinaryOp : public Expression {
public:
    enum class Operator {
        ADD, SUB, MUL, DIV, MOD,
        EQ, NE, LT, GT, LE, GE,
        AND, OR
    };

    Operator op;
    ExprPtr left;
    ExprPtr right;

    BinaryOp(Operator o, ExprPtr l, ExprPtr r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
};

class UnaryOp : public Expression {
public:
    enum class Operator {
        NOT, MINUS
    };

    Operator op;
    ExprPtr operand;

    UnaryOp(Operator o, ExprPtr e)
        : op(o), operand(std::move(e)) {}
};

class MemberAccess : public Expression {
public:
    ExprPtr object;
    std::string memberName;

    MemberAccess(ExprPtr obj, std::string mem)
        : object(std::move(obj)), memberName(std::move(mem)) {}
};

class FunctionCall : public Expression {
public:
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
    bool isBuiltIn;

    FunctionCall(ExprPtr c, std::vector<ExprPtr> args, bool builtIn = false)
        : callee(std::move(c)), arguments(std::move(args)), isBuiltIn(builtIn) {}
};

class ListAccess : public Expression {
public:
    ExprPtr list;
    ExprPtr index;

    ListAccess(ExprPtr l, ExprPtr idx)
        : list(std::move(l)), index(std::move(idx)) {}
};

// Statement Nodes
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

class ExpressionStatement : public Statement {
public:
    ExprPtr expression;

    explicit ExpressionStatement(ExprPtr expr)
        : expression(std::move(expr)) {}
};

class VarDecl : public Statement {
public:
    bool isConst;
    TypePtr type;
    std::string name;
    ExprPtr initializer;

    VarDecl(bool ic, TypePtr t, std::string n, ExprPtr init)
        : isConst(ic), type(std::move(t)), name(std::move(n)),
          initializer(std::move(init)) {}
};

class Assignment : public Statement, public Expression {
public:
    ExprPtr target;
    ExprPtr value;

    Assignment(ExprPtr t, ExprPtr v)
        : target(std::move(t)), value(std::move(v)) {}
};

class If : public Statement {
public:
    ExprPtr condition;
    std::vector<StmtPtr> thenBlock;
    std::vector<std::pair<ExprPtr, std::vector<StmtPtr>>> elifBlocks;
    std::vector<StmtPtr> elseBlock;

    If(ExprPtr cond, std::vector<StmtPtr> tb,
       std::vector<std::pair<ExprPtr, std::vector<StmtPtr>>> eb = {},
       std::vector<StmtPtr> else_block = {})
        : condition(std::move(cond)), thenBlock(std::move(tb)),
          elifBlocks(std::move(eb)), elseBlock(std::move(else_block)) {}
};

class While : public Statement {
public:
    ExprPtr condition;
    std::vector<StmtPtr> body;

    While(ExprPtr cond, std::vector<StmtPtr> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

class For : public Statement {
public:
    std::string iterator;
    ExprPtr start;
    ExprPtr end;
    ExprPtr step;
    std::vector<StmtPtr> body;

    For(std::string it, ExprPtr s, ExprPtr e, ExprPtr st, std::vector<StmtPtr> b)
        : iterator(std::move(it)), start(std::move(s)), end(std::move(e)),
          step(std::move(st)), body(std::move(b)) {}
};

class Function : public Statement {
public:
    std::string name;
    std::vector<std::pair<TypePtr, std::string>> parameters;
    std::vector<StmtPtr> body;
    TypePtr returnType;

    Function(std::string n, std::vector<std::pair<TypePtr, std::string>> params,
             std::vector<StmtPtr> b, TypePtr rt = nullptr)
        : name(std::move(n)), parameters(std::move(params)),
          body(std::move(b)), returnType(std::move(rt)) {}
};

class Return : public Statement {
public:
    ExprPtr value;

    explicit Return(ExprPtr v)
        : value(std::move(v)) {}
};

class TryCatch : public Statement {
public:
    std::vector<StmtPtr> tryBlock;
    std::string exceptionName;
    std::vector<StmtPtr> catchBlock;

    TryCatch(std::vector<StmtPtr> tb, std::string en, std::vector<StmtPtr> cb)
        : tryBlock(std::move(tb)), exceptionName(std::move(en)), catchBlock(std::move(cb)) {}
};

class Print : public Statement {
public:
    ExprPtr expression;

    explicit Print(ExprPtr expr)
        : expression(std::move(expr)) {}
};

class Input : public Statement {
public:
    ExprPtr prompt;

    explicit Input(ExprPtr p)
        : prompt(std::move(p)) {}
};

// Program root node
class Program : public ASTNode {
public:
    std::vector<StmtPtr> statements;

    explicit Program(std::vector<StmtPtr> stmts)
        : statements(std::move(stmts)) {}
};

class ENDLOOP : public Statement {
public:
    ENDLOOP() = default;
};

class NEXT : public Statement {
public:
    NEXT() = default;
};

