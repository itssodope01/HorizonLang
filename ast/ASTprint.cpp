#include "../lexer/Lexer.h"
#include "AST.h"
#include <iostream>
#include <memory>

namespace ASTPrinter {
    static void printIndent(int indent) {
        for (int i = 0; i < indent; ++i) {
            std::cout << "  ";
        }
    }

    static std::string typeKindToString(Type::Kind kind) {
        switch (kind) {
            case Type::Kind::INT: return "int";
            case Type::Kind::FLOAT: return "float";
            case Type::Kind::STRING: return "string";
            case Type::Kind::BOOL: return "bool";
            case Type::Kind::LIST: return "list";
            case Type::Kind::VOID: return "void";
            default: return "unknown";
        }
    }

    static std::string binaryOperatorToString(BinaryOp::Operator op) {
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

    static std::string unaryOperatorToString(UnaryOp::Operator op) {
        switch (op) {
            case UnaryOp::Operator::NOT:   return "not";
            case UnaryOp::Operator::MINUS: return "-";
            default: return "?";
        }
    }

    void printAST(const std::shared_ptr<ASTNode>& node, int indent) {
        if (!node) return;

        if (auto program = std::dynamic_pointer_cast<Program>(node)) {
            printIndent(indent);
            std::cout << "Program" << std::endl;
            for (const auto& stmt : program->statements) {
                printAST(stmt, indent + 1);
            }
        } else if (auto func = std::dynamic_pointer_cast<Function>(node)) {
            printIndent(indent);
            std::cout << "Function: " << func->name << std::endl;
            printIndent(indent + 1);
            std::cout << "Parameters:" << std::endl;
            for (const auto& param : func->parameters) {
                printIndent(indent + 2);
                std::cout << param.second << " : " << typeKindToString(param.first->kind) << std::endl;
            }
            printIndent(indent + 1);
            std::cout << "Body:" << std::endl;
            for (const auto& stmt : func->body) {
                printAST(stmt, indent + 2);
            }
        } else if (auto varDecl = std::dynamic_pointer_cast<VarDecl>(node)) {
            printIndent(indent);
            std::cout << (varDecl->isConst ? "Const " : "Var ") << varDecl->name
                      << " : " << typeKindToString(varDecl->type->kind) << std::endl;
            if (varDecl->initializer) {
                printIndent(indent + 1);
                std::cout << "Initializer:" << std::endl;
                printAST(varDecl->initializer, indent + 2);
            }
        } else if (auto assign = std::dynamic_pointer_cast<Assignment>(node)) {
            printIndent(indent);
            std::cout << "Assignment" << std::endl;
            printIndent(indent + 1);
            std::cout << "Target:" << std::endl;
            printAST(assign->target, indent + 2);
            printIndent(indent + 1);
            std::cout << "Value:" << std::endl;
            printAST(assign->value, indent + 2);
        } else if (auto printStmt = std::dynamic_pointer_cast<Print>(node)) {
            printIndent(indent);
            std::cout << "Print" << std::endl;
            printAST(printStmt->expression, indent + 1);
        } else if (auto inputStmt = std::dynamic_pointer_cast<Input>(node)) {
            printIndent(indent);
            std::cout << "Input" << std::endl;
            printAST(inputStmt->prompt, indent + 1);
        } else if (auto forStmt = std::dynamic_pointer_cast<For>(node)) {
            printIndent(indent);
            std::cout << "For Loop with iterator: " << forStmt->iterator << std::endl;
            printIndent(indent + 1);
            std::cout << "Start:" << std::endl;
            printAST(forStmt->start, indent + 2);
            printIndent(indent + 1);
            std::cout << "End:" << std::endl;
            printAST(forStmt->end, indent + 2);
            if (forStmt->step) {
                printIndent(indent + 1);
                std::cout << "Step:" << std::endl;
                printAST(forStmt->step, indent + 2);
            }
            printIndent(indent + 1);
            std::cout << "Body:" << std::endl;
            for (const auto& stmt : forStmt->body) {
                printAST(stmt, indent + 2);
            }
        } else if (auto whileStmt = std::dynamic_pointer_cast<While>(node)) {
            printIndent(indent);
            std::cout << "While" << std::endl;
            printIndent(indent + 1);
            std::cout << "Condition:" << std::endl;
            printAST(whileStmt->condition, indent + 2);
            printIndent(indent + 1);
            std::cout << "Body:" << std::endl;
            for (const auto& stmt : whileStmt->body) {
                printAST(stmt, indent + 2);
            }
        } else if (auto ifStmt = std::dynamic_pointer_cast<If>(node)) {
            printIndent(indent);
            std::cout << "If Statement" << std::endl;
            printIndent(indent + 1);
            std::cout << "Condition:" << std::endl;
            printAST(ifStmt->condition, indent + 2);
            printIndent(indent + 1);
            std::cout << "Then Branch:" << std::endl;
            for (const auto& stmt : ifStmt->thenBlock) {
                printAST(stmt, indent + 2);
            }
            if (!ifStmt->elseBlock.empty()) {
                printIndent(indent + 1);
                std::cout << "Else Branch:" << std::endl;
                for (const auto& stmt : ifStmt->elseBlock) {
                    printAST(stmt, indent + 2);
                }
            }
        } else if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStatement>(node)) {
            printIndent(indent);
            std::cout << "Expression Statement:" << std::endl;
            printAST(exprStmt->expression, indent + 1);
        } else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(node)) {
            printIndent(indent);
            std::cout << "Function Call" << std::endl;
            printIndent(indent + 1);
            std::cout << "Callee:" << std::endl;
            printAST(funcCall->callee, indent + 2);
            printIndent(indent + 1);
            std::cout << "Arguments:" << std::endl;
            for (const auto& arg : funcCall->arguments) {
                printAST(arg, indent + 2);
            }
        } else if (auto ident = std::dynamic_pointer_cast<Identifier>(node)) {
            printIndent(indent);
            std::cout << "Identifier: " << ident->name << std::endl;
        } else if (auto literal = std::dynamic_pointer_cast<Literal>(node)) {
            printIndent(indent);
            std::cout << "Literal: ";

            // Updated lambda function to handle different types
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::vector<ExprPtr>>) {
                    std::cout << "[\n";
                    for (const auto& elem : arg) {
                        printAST(elem, indent + 1);
                    }
                    printIndent(indent);
                    std::cout << "]";
                } else {
                    std::cout << arg;
                }
            }, literal->value);

            std::cout << std::endl;
        } else if (auto binaryOp = std::dynamic_pointer_cast<BinaryOp>(node)) {
            printIndent(indent);
            std::cout << "Binary Operation: " << binaryOperatorToString(binaryOp->op) << std::endl;
            printIndent(indent + 1);
            std::cout << "Left:" << std::endl;
            printAST(binaryOp->left, indent + 2);
            printIndent(indent + 1);
            std::cout << "Right:" << std::endl;
            printAST(binaryOp->right, indent + 2);
        } else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOp>(node)) {
            printIndent(indent);
            std::cout << "Unary Operation: " << unaryOperatorToString(unaryOp->op) << std::endl;
            printIndent(indent + 1);
            std::cout << "Operand:" << std::endl;
            printAST(unaryOp->operand, indent + 2);
        } else if (auto memberAccess = std::dynamic_pointer_cast<MemberAccess>(node)) {
            printIndent(indent);
            std::cout << "Member Access" << std::endl;
            printIndent(indent + 1);
            std::cout << "Object:" << std::endl;
            printAST(memberAccess->object, indent + 2);
            printIndent(indent + 1);
            std::cout << "Member Name: " << memberAccess->memberName << std::endl;
        } else if (auto listAccess = std::dynamic_pointer_cast<ListAccess>(node)) {
            printIndent(indent);
            std::cout << "List Access" << std::endl;
            printIndent(indent + 1);
            std::cout << "List:" << std::endl;
            printAST(listAccess->list, indent + 2);
            printIndent(indent + 1);
            std::cout << "Index:" << std::endl;
            printAST(listAccess->index, indent + 2);
        } else if (auto tryCatch = std::dynamic_pointer_cast<TryCatch>(node)) {
            printIndent(indent);
            std::cout << "Try-Catch Block" << std::endl;

            printIndent(indent + 1);
            std::cout << "Try Block:" << std::endl;
            for (const auto& stmt : tryCatch->tryBlock) {
                printAST(stmt, indent + 2);
            }

            printIndent(indent + 1);
            std::cout << "Exception Name: " << tryCatch->exceptionName << std::endl;

            printIndent(indent + 1);
            std::cout << "Catch Block:" << std::endl;
            for (const auto& stmt : tryCatch->catchBlock) {
                printAST(stmt, indent + 2);
            }
        } else if (auto returnStmt = std::dynamic_pointer_cast<Return>(node)) {
            printIndent(indent);
            std::cout << "Return Statement" << std::endl;
            if (returnStmt->value) {
                printIndent(indent + 1);
                std::cout << "Value:" << std::endl;
                printAST(returnStmt->value, indent + 2);
            }
        } else {
            printIndent(indent);
            std::cout << "Unknown node type" << std::endl;
        }
    }
}
