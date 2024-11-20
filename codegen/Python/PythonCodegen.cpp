#include "PythonCodegen.h"
#include <fstream>
#include <stdexcept>

// built-in functions mapping
const std::unordered_map<std::string, std::string> PythonCodeGen::builtinFunctions = {
    {"length", "len"},
    {"append", "append"},
    {"prepend", "insert"},
    {"remove", "pop"},
    {"empty", "clear"},
    {"STR", "str"},
    {"input", "input"},
    {"INT", "int"},
    {"FLOAT", "float"},
};

// Helper methods for indentation
void PythonCodeGen::indent() {
    ++indentLevel;
    updateIndent();
}

void PythonCodeGen::dedent() {
    if (indentLevel > 0) {
        --indentLevel;
    }
    updateIndent();
}

void PythonCodeGen::updateIndent() {
    currentIndent = std::string(indentLevel * 4, ' ');
}


std::string PythonCodeGen::generate(const std::shared_ptr<Program>& program) {
    // Python imports
    output << "# Generated Python code\n";
    output << "from typing import List, Any\n\n";

    // Generating program statements
    for (const auto& stmt : program->statements) {
        generateStatement(stmt);
        output << "\n";
    }

    return output.str();
}

// single statement
void PythonCodeGen::generateStatement(const StmtPtr& stmt) {
    if (auto varDecl = std::dynamic_pointer_cast<VarDecl>(stmt)) {
        generateVarDecl(varDecl);
    }
    else if (auto func = std::dynamic_pointer_cast<Function>(stmt)) {
        generateFunction(func);
    }
    else if (auto printStmt = std::dynamic_pointer_cast<Print>(stmt)) {
        generatePrint(printStmt);
    }
    else if (auto tryCatch = std::dynamic_pointer_cast<TryCatch>(stmt)) {
        generateTryCatch(tryCatch);
    }
    else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
        generateAssignment(assign);
    }
    else if (auto ifStmt = std::dynamic_pointer_cast<If>(stmt)) {
        generateIfStatement(ifStmt);
    }
    else if (auto whileStmt = std::dynamic_pointer_cast<While>(stmt)) {
        generateWhileLoop(whileStmt);
    }
    else if (auto forLoop = std::dynamic_pointer_cast<For>(stmt)) {
        generateForLoop(forLoop);
    }
    else if (auto returnStmt = std::dynamic_pointer_cast<Return>(stmt)) {
        generateReturnStatement(returnStmt);
    }
    else if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt)) {
        generateExpressionStatement(exprStmt);
    }
    else if (auto endLoop = std::dynamic_pointer_cast<ENDLOOP>(stmt)) {
        output << currentIndent << "break\n";
    }
    else if (auto nextStmt = std::dynamic_pointer_cast<NEXT>(stmt)) {
        output << currentIndent << "continue\n";
    }
    else {
        // other statement
    }
}

// variable declaration
void PythonCodeGen::generateVarDecl(const std::shared_ptr<VarDecl>& varDecl) {
    output << currentIndent << varDecl->name << " = ";
    if (varDecl->initializer) {
        generateExpression(varDecl->initializer);
    } else {
        output << "None";
    }
    output << "\n";

    // type information
    variableTypes[varDecl->name] = varDecl->type;
}

// function definition
void PythonCodeGen::generateFunction(const std::shared_ptr<Function>& func) {
    output << currentIndent << "def " << func->name << "(";

    // parameters
    bool first = true;
    for (const auto& param : func->parameters) {
        if (!first) output << ", ";
        output << param.second << ": " << convertType(param.first);
        first = false;
    }
    output << ")";

    if (func->returnType && func->returnType->kind != Type::Kind::VOID) {
        output << " -> " << convertType(func->returnType);
    }

    output << ":\n";

    // function body
    indent();
    if (func->body.empty()) {
        output << currentIndent << "pass\n";
    } else {
        for (const auto& stmt : func->body) {
            generateStatement(stmt);
        }
    }
    dedent();
    output << "\n";
}

// assignment statement
void PythonCodeGen::generateAssignment(const std::shared_ptr<Assignment>& assign) {
    output << currentIndent;
    generateExpression(assign->target);
    output << " = ";
    generateExpression(assign->value);
    output << "\n";
}

// expression
void PythonCodeGen::generateExpression(const ExprPtr& expr) {
    if (auto literal = std::dynamic_pointer_cast<Literal>(expr)) {
        if (std::holds_alternative<int>(literal->value)) {
            output << std::get<int>(literal->value);
        }
        else if (std::holds_alternative<float>(literal->value)) {
            output << std::get<float>(literal->value);
        }
        else if (std::holds_alternative<std::string>(literal->value)) {
            output << std::get<std::string>(literal->value);
        }
        else if (std::holds_alternative<bool>(literal->value)) {
            output << (std::get<bool>(literal->value) ? "True" : "False");
        }
        // list literals
        else if (std::holds_alternative<std::vector<ExprPtr>>(literal->value)) {
            output << "[";
            const auto& elements = std::get<std::vector<ExprPtr>>(literal->value);
            for (size_t i = 0; i < elements.size(); ++i) {
                generateExpression(elements[i]);
                if (i < elements.size() - 1) {
                    output << ", ";
                }
            }
            output << "]";
        }
    }
    else if (auto identifier = std::dynamic_pointer_cast<Identifier>(expr)) {
        output << identifier->name;
    }
    else if (auto binaryOp = std::dynamic_pointer_cast<BinaryOp>(expr)) {
        generateBinaryOp(binaryOp);
    }
    else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOp>(expr)) {
        generateUnaryOp(unaryOp);
    }
    else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(expr)) {
        generateFunctionCall(funcCall);
    }
    else if (auto listAccess = std::dynamic_pointer_cast<ListAccess>(expr)) {
        generateListAccess(listAccess);
    }
    else if (auto memberAccess = std::dynamic_pointer_cast<MemberAccess>(expr)) {
        generateMemberAccess(memberAccess);
    }
    else if (auto assign = std::dynamic_pointer_cast<Assignment>(expr)) {
        generateAssignment(assign);
    }
    else {
        // Handle other expression types as needed
    }
}

// binary operation
void PythonCodeGen::generateBinaryOp(const std::shared_ptr<BinaryOp>& binOp) {
    output << "(";
    generateExpression(binOp->left);
    output << " " << binaryOperatorToString(binOp->op) << " ";
    generateExpression(binOp->right);
    output << ")";
}

std::string PythonCodeGen::binaryOperatorToString(BinaryOp::Operator op) {
    switch (op) {
        case BinaryOp::Operator::ADD: return "+";
        case BinaryOp::Operator::SUB: return "-";
        case BinaryOp::Operator::MUL: return "*";
        case BinaryOp::Operator::DIV: return "/";
        case BinaryOp::Operator::MOD: return "%";
        case BinaryOp::Operator::EQ:  return "==";
        case BinaryOp::Operator::NE:  return "!=";
        case BinaryOp::Operator::LT:  return "<";
        case BinaryOp::Operator::GT:  return ">";
        case BinaryOp::Operator::LE:  return "<=";
        case BinaryOp::Operator::GE:  return ">=";
        case BinaryOp::Operator::AND: return "and";
        case BinaryOp::Operator::OR:  return "or";
        default: return "?";
    }
}

// unary operation
void PythonCodeGen::generateUnaryOp(const std::shared_ptr<UnaryOp>& unaryOp) {
    output << unaryOperatorToString(unaryOp->op);
    generateExpression(unaryOp->operand);
}

std::string PythonCodeGen::unaryOperatorToString(UnaryOp::Operator op) {
    switch (op) {
        case UnaryOp::Operator::NOT: return "not ";
        case UnaryOp::Operator::MINUS: return "-";
        default: return "?";
    }
}

// function call
void PythonCodeGen::generateFunctionCall(const std::shared_ptr<FunctionCall>& call) {
    if (auto memberAccess = std::dynamic_pointer_cast<MemberAccess>(call->callee)) {
        // Handle method calls on objects
        std::string methodName = memberAccess->memberName;

        if (methodName == "length") {
            // len(obj)
            output << "len(";
            generateExpression(memberAccess->object);
            output << ")";
        }
        else if (methodName == "substring") {
            // obj[start:end]
            generateExpression(memberAccess->object);
            output << "[";
            generateExpression(call->arguments[0]); // start
            output << ":";
            generateExpression(call->arguments[1]); // end
            output << "]";
        }
        else if (methodName == "concat") {
            // obj + arg0
            generateExpression(memberAccess->object);
            output << " + ";
            generateExpression(call->arguments[0]);
        }
        else {

            auto it = builtinFunctions.find(methodName);
            if (it != builtinFunctions.end()) {
                if (methodName == "append") {
                    // obj.append(arg0)
                    generateExpression(memberAccess->object);
                    output << ".append(";
                    generateExpression(call->arguments[0]);
                    output << ")";
                }
                else if (methodName == "prepend") {
                    // obj.insert(0, arg0)
                    generateExpression(memberAccess->object);
                    output << ".insert(0, ";
                    generateExpression(call->arguments[0]);
                    output << ")";
                }
                else if (methodName == "remove") {
                    // obj.pop(arg0)
                    generateExpression(memberAccess->object);
                    output << ".pop(";
                    generateExpression(call->arguments[0]);
                    output << ")";
                }
                else if (methodName == "empty") {
                    // obj.clear()
                    generateExpression(memberAccess->object);
                    output << ".clear()";
                }
                else {
                    // Other built-in methods
                    generateExpression(memberAccess->object);
                    output << ".";
                    output << it->second << "(";
                    for (size_t i = 0; i < call->arguments.size(); ++i) {
                        generateExpression(call->arguments[i]);
                        if (i < call->arguments.size() - 1) {
                            output << ", ";
                        }
                    }
                    output << ")";
                }
            }
            else {
                // Unknown method
                generateExpression(memberAccess->object);
                output << ".";
                output << methodName << "(";
                for (size_t i = 0; i < call->arguments.size(); ++i) {
                    generateExpression(call->arguments[i]);
                    if (i < call->arguments.size() - 1) {
                        output << ", ";
                    }
                }
                output << ")";
            }
        }
    }
    else if (auto identifier = std::dynamic_pointer_cast<Identifier>(call->callee)) {
        std::string functionName = identifier->name;

        auto it = builtinFunctions.find(functionName);
        if (it != builtinFunctions.end()) {
            output << it->second << "(";
            for (size_t i = 0; i < call->arguments.size(); ++i) {
                generateExpression(call->arguments[i]);
                if (i < call->arguments.size() - 1) {
                    output << ", ";
                }
            }
            output << ")";
        }
        else {
            // Regular function call
            output << functionName << "(";
            for (size_t i = 0; i < call->arguments.size(); ++i) {
                generateExpression(call->arguments[i]);
                if (i < call->arguments.size() - 1) {
                    output << ", ";
                }
            }
            output << ")";
        }
    }
    else {
        // Other cases
        generateExpression(call->callee);
        output << "(";
        for (size_t i = 0; i < call->arguments.size(); ++i) {
            generateExpression(call->arguments[i]);
            if (i < call->arguments.size() - 1) {
                output << ", ";
            }
        }
        output << ")";
    }
}

// list access
void PythonCodeGen::generateListAccess(const std::shared_ptr<ListAccess>& listAccess) {
    generateExpression(listAccess->list);
    output << "[";
    generateExpression(listAccess->index);
    output << "]";
}

// member access
void PythonCodeGen::generateMemberAccess(const std::shared_ptr<MemberAccess>& memberAccess) {
    generateExpression(memberAccess->object);
    output << "." << memberAccess->memberName;
}

// try-catch block
void PythonCodeGen::generateTryCatch(const std::shared_ptr<TryCatch>& tryCatch) {
    output << currentIndent << "try:\n";
    indent();
    for (const auto& stmt : tryCatch->tryBlock) {
        generateStatement(stmt);
    }
    dedent();
    output << currentIndent << "except Exception as " << tryCatch->exceptionName << ":\n";
    indent();
    for (const auto& stmt : tryCatch->catchBlock) {
        generateStatement(stmt);
    }
    dedent();
}

void PythonCodeGen::generateForLoop(const std::shared_ptr<For>& forLoop) {
    output << currentIndent << "for " << forLoop->iterator << " in range(";

    // start and end expressions
    generateExpression(forLoop->start);
    output << ", ";
    generateExpression(forLoop->end);

    output << ", ";

    // step based on start and end values
    if (forLoop->step) {
        // We need to check if the start and end are literals
        auto startLiteral = std::dynamic_pointer_cast<Literal>(forLoop->start);
        auto endLiteral = std::dynamic_pointer_cast<Literal>(forLoop->end);
        auto stepLiteral = std::dynamic_pointer_cast<Literal>(forLoop->step);

        if (startLiteral && endLiteral && stepLiteral) {

            int startValue = std::get<int>(startLiteral->value);
            int endValue = std::get<int>(endLiteral->value);
            int stepValue = std::get<int>(stepLiteral->value);

            if ((startValue > endValue && stepValue > 0) || (startValue < endValue && stepValue < 0)) {
                // Negate the step
                output << -stepValue;
            } else {
                output << stepValue;
            }
        } else {

            output << "(";
            generateExpression(forLoop->step);
            output << " if (";
            generateExpression(forLoop->start);
            output << ") < (";
            generateExpression(forLoop->end);
            output << ") else -(";
            generateExpression(forLoop->step);
            output << "))";
        }
    } else {

        output << "(";
        output << "1 if (";
        generateExpression(forLoop->start);
        output << ") < (";
        generateExpression(forLoop->end);
        output << ") else -1)";
    }

    output << "):\n";

    indent();
    for (const auto& stmt : forLoop->body) {
        generateStatement(stmt);
    }
    dedent();
}


// Generate while loop
void PythonCodeGen::generateWhileLoop(const std::shared_ptr<While>& whileLoop) {
    output << currentIndent << "while ";
    generateExpression(whileLoop->condition);
    output << ":\n";
    indent();
    for (const auto& stmt : whileLoop->body) {
        generateStatement(stmt);
    }
    dedent();
}


void PythonCodeGen::generateIfStatement(const std::shared_ptr<If>& ifStmt) {
    output << currentIndent << "if ";
    generateExpression(ifStmt->condition);
    output << ":\n";
    indent();
    for (const auto& stmt : ifStmt->thenBlock) {
        generateStatement(stmt);
    }
    dedent();

    // Handle elif blocks
    for (const auto& elifPair : ifStmt->elifBlocks) {
        output << currentIndent << "elseif ";
        generateExpression(elifPair.first);  // elif condition
        output << ":\n";
        indent();
        for (const auto& stmt : elifPair.second) {  // elif body
            generateStatement(stmt);
        }
        dedent();
    }

    // Handle else block
    if (!ifStmt->elseBlock.empty()) {
        output << currentIndent << "else:\n";
        indent();
        for (const auto& stmt : ifStmt->elseBlock) {
            generateStatement(stmt);
        }
        dedent();
    }
}


void PythonCodeGen::generateReturnStatement(const std::shared_ptr<Return>& returnStmt) {
    output << currentIndent << "return";
    if (returnStmt->value) {
        output << " ";
        generateExpression(returnStmt->value);
    }
    output << "\n";
}


void PythonCodeGen::generatePrint(const std::shared_ptr<Print>& printStmt) {
    output << currentIndent << "print(";
    generateExpression(printStmt->expression);
    output << ")\n";
}


void PythonCodeGen::generateExpressionStatement(const std::shared_ptr<ExpressionStatement>& exprStmt) {

    if (auto assign = std::dynamic_pointer_cast<Assignment>(exprStmt->expression)) {
        generateAssignment(assign);
    }
    else {
        output << currentIndent;
        generateExpression(exprStmt->expression);
        output << "\n";
    }
}

std::string PythonCodeGen::convertType(const TypePtr& type) {
    if (!type) return "Any";

    switch (type->kind) {
        case Type::Kind::INT:
            return "int";
        case Type::Kind::FLOAT:
            return "float";
        case Type::Kind::STRING:
            return "str";
        case Type::Kind::BOOL:
            return "bool";
        case Type::Kind::LIST:
            if (type->elementType) {
                return "List[" + convertType(type->elementType) + "]";
            }
            else {
                return "List[Any]";
            }
        case Type::Kind::VOID:
            return "None";
        default:
            return "Any";
    }
}

void PythonCodeGen::writeToFile(const std::string& filename, const std::string& code) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << code;
        file.close();
    }
    else {
        throw std::runtime_error("Unable to open file: " + filename);
    }
}
