#pragma once

#include <string>
#include <memory>
#include <sstream>
#include "../../ast/AST.h"

class CppCodeGen {
public:
    std::string generate(std::shared_ptr<Program> program);

    void generateSafeNegate(const ExprPtr &expr);

    void writeToFile(const std::string& filename, const std::string& code);

private:
    std::stringstream codeStream;

    // Helper methods
    void generateStatement(const StmtPtr& stmt);
    void generateExpression(const ExprPtr& expr);
    void generateType(const TypePtr& type);
    void generateFunctionDefinition(const std::shared_ptr<Function>& funcDef);

    // Utility methods
    std::string getOperatorString(BinaryOp::Operator op);

    void indent();
    bool isNegativeLiteral(const ExprPtr& expr);
    TypePtr inferFunctionReturnType(const std::shared_ptr<Function>& funcDef);

    // Indentation level
    int indentLevel = 1;
};
