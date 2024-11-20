#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include "../ast/AST.h"
#include "../ast/ASTprint.h"

class SemanticAnalyzer {
public:
    bool analyze(const std::shared_ptr<Program>& program);
    const std::vector<std::string>& getErrors() const;

private:
    std::vector<std::string> errors;
    std::unordered_map<std::string, TypePtr> constTable;
    std::vector<std::unordered_map<std::string, TypePtr>> symbolTableStack;
    std::vector<std::unordered_map<std::string, bool>> variableInitializedStack;
    std::optional<TypePtr> currentFunctionReturnType;
    std::string currentFunctionName;
    std::unordered_map<std::string, std::vector<TypePtr>> functionSignatures;
    std::unordered_map<std::string, TypePtr> functionReturnTypes;

    bool hasReturnStatement = false;

    void addError(const std::string& message);
    void checkStatement(const std::shared_ptr<Statement>& stmt);
    TypePtr checkExpression(const std::shared_ptr<Expression>& expr);
    void checkVarDeclaration(const std::shared_ptr<VarDecl>& varDecl);

    void checkAssignment(const std::shared_ptr<Assignment>& assignment);
    void checkFunction(const std::shared_ptr<Function>& function);
    TypePtr checkFunctionCall(const std::shared_ptr<FunctionCall>& funcCall);
    void checkListOperation(const std::shared_ptr<ListAccess>& listOp);
    void checkIfStatement(const std::shared_ptr<If>& ifStmt);
    void checkWhileStatement(const std::shared_ptr<While>& whileStmt);
    void checkForLoop(const std::shared_ptr<For>& forLoop);
    void checkReturnStatement(const std::shared_ptr<Return>& returnStmt);
    void registerFunction(const std::shared_ptr<Function>& function);
    bool areTypesCompatible(TypePtr expected, TypePtr actual) const;
    TypePtr getTypeFromBinaryOp(BinaryOp::Operator op, TypePtr left, TypePtr right);
    TypePtr getTypeFromUnaryOp(UnaryOp::Operator op, TypePtr operand);
    static bool isNumericType(const TypePtr& type);

    void enterScope();
    void exitScope();
    int loopDepth = 0;
    std::unordered_map<std::string, TypePtr>& currentScope();
    std::unordered_map<std::string, bool>& currentInitializedScope();
    bool isVariableInitialized(const std::string& name) const;
    TypePtr lookupVariable(const std::string& name);
    bool isVariableDeclared(const std::string& name) const;
};
