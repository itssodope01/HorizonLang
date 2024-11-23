#pragma once

#include "../../ast/AST.h"
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string>
#include <set>

class PythonCodeGen {
private:
    std::stringstream output;
    int indentLevel = 0;
    std::string currentIndent;

    // Cache for variable types
    std::unordered_map<std::string, TypePtr> variableTypes;

    // Set of global variables
    std::set<std::string> globalVariables;

    // Helper methods
    void indent();
    void dedent();
    void updateIndent();

    // Code generation methods
    void generateExpression(const ExprPtr& expr);
    void generateStatement(const StmtPtr& stmt);
    void generateFunction(const std::shared_ptr<Function>& func);
    void generateVarDecl(const std::shared_ptr<VarDecl>& varDecl);
    void generateAssignment(const std::shared_ptr<Assignment>& assign);
    void generateBinaryOp(const std::shared_ptr<BinaryOp>& binOp);
    void generateUnaryOp(const std::shared_ptr<UnaryOp>& unaryOp);
    void generateFunctionCall(const std::shared_ptr<FunctionCall>& call);
    void generateListAccess(const std::shared_ptr<ListAccess>& listAccess);
    void generateMemberAccess(const std::shared_ptr<MemberAccess>& memberAccess);
    void generateTryCatch(const std::shared_ptr<TryCatch>& tryCatch);
    void generateForLoop(const std::shared_ptr<For>& forLoop);
    void generateWhileLoop(const std::shared_ptr<While>& whileLoop);
    void generateIfStatement(const std::shared_ptr<If>& ifStmt);
    void generateReturnStatement(const std::shared_ptr<Return>& returnStmt);
    void generatePrint(const std::shared_ptr<Print>& printStmt);
    void generateExpressionStatement(const std::shared_ptr<ExpressionStatement>& exprStmt);

    // Type conversion helpers
    std::string convertType(const TypePtr& type);

    std::string getObjectName(const ExprPtr &expr);

    std::string binaryOperatorToString(BinaryOp::Operator op);
    std::string unaryOperatorToString(UnaryOp::Operator op);

    // Standard library mapping
    static const std::unordered_map<std::string, std::string> builtinFunctions;

    // Helper to collect assigned variables
    void collectAssignedVariables(const StmtPtr& stmt, std::set<std::string>& assignedVars);
    void collectAssignedVariables(const ExprPtr& expr, std::set<std::string>& assignedVars);

    // Flag to indicate if we are inside a function
    bool inFunction = false;

public:
    PythonCodeGen() = default;

    // Main generation method
    std::string generate(const std::shared_ptr<Program>& program);


    void writeToFile(const std::string& filename, const std::string& code);
};
