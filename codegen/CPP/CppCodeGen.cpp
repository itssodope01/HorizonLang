#include "CppCodeGen.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

std::string CppCodeGen::generate(std::shared_ptr<Program> program) {
    // Include necessary headers
    codeStream << "#include <iostream>\n";
    codeStream << "#include <string>\n";
    codeStream << "#include <vector>\n";
    codeStream << "#include <stdexcept>\n";
    codeStream << "#include <algorithm>\n";
    codeStream << "\n";

    // Safe divide function
    codeStream << "int safe_divide(int numerator, int denominator) {\n";
    codeStream << "    if (denominator == 0) {\n";
    codeStream << "        throw std::runtime_error(\"Division by zero\");\n";
    codeStream << "    }\n";
    codeStream << "    return numerator / denominator;\n";
    codeStream << "}\n\n";

    // Collect global variable declarations
    std::vector<std::shared_ptr<VarDecl>> globalVars;
    std::vector<StmtPtr> topLevelStatements;
    std::unordered_map<std::string, std::string> functionRenames;

    // First pass: Collect information
    for (const auto& stmt : program->statements) {
        if (auto varDecl = std::dynamic_pointer_cast<VarDecl>(stmt)) {
            globalVars.push_back(varDecl);
        } else if (auto funcDef = std::dynamic_pointer_cast<Function>(stmt)) {
            // Rename main function if necessary
            if (funcDef->name == "main") {
                functionRenames["main"] = "hl_main";
                funcDef->name = "hl_main";
            }
        } else {
            topLevelStatements.push_back(stmt);
        }
    }

    // Declare global variables
    for (const auto& varDecl : globalVars) {
        codeStream << "\n";
        if (varDecl->isConst) {
            codeStream << "const ";
        }
        generateType(varDecl->type);
        codeStream << " " << varDecl->name;

        // Initialize const variables at declaration
        if (varDecl->isConst && varDecl->initializer) {
            codeStream << " = ";
            generateExpression(varDecl->initializer);
        }

        codeStream << ";\n";
    }

    // Function definitions
    for (const auto& stmt : program->statements) {
        if (auto funcDef = std::dynamic_pointer_cast<Function>(stmt)) {
            generateFunctionDefinition(funcDef);
        }
    }

    // Main function
    codeStream << "\nint main() {\n";
    indentLevel++;

    // Initialize non-const global variables within main
    for (const auto& varDecl : globalVars) {
        if (!varDecl->isConst && varDecl->initializer) {
            indent();
            codeStream << varDecl->name << " = ";
            generateExpression(varDecl->initializer);
            codeStream << ";\n";
        }
    }

    // Generate top-level statements
    for (const auto& stmt : topLevelStatements) {
        generateStatement(stmt);
    }

    indent();
    codeStream << "return 0;\n";
    indentLevel--;
    codeStream << "}\n";

    return codeStream.str();
}

void CppCodeGen::generateFunctionDefinition(const std::shared_ptr<Function>& funcDef) {
    codeStream << "\n";
    TypePtr returnType = funcDef->returnType;
    if (!returnType) {
        returnType = inferFunctionReturnType(funcDef);
    }
    generateType(returnType);
    codeStream << " " << funcDef->name << "(";
    for (size_t i = 0; i < funcDef->parameters.size(); ++i) {
        generateType(funcDef->parameters[i].first);
        codeStream << " " << funcDef->parameters[i].second;
        if (i < funcDef->parameters.size() - 1) {
            codeStream << ", ";
        }
    }
    codeStream << ") {\n";
    indentLevel++;
    for (const auto& s : funcDef->body) {
        generateStatement(s);
    }
    indentLevel--;
    codeStream << "}\n";
}

void CppCodeGen::generateStatement(const StmtPtr& stmt) {
    if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt)) {
        indent();
        generateExpression(exprStmt->expression);
        codeStream << ";\n";
    } else if (auto varDecl = std::dynamic_pointer_cast<VarDecl>(stmt)) {
        indent();
        if (varDecl->isConst) {
            codeStream << "const ";
        }
        generateType(varDecl->type);
        codeStream << " " << varDecl->name;
        if (varDecl->initializer) {
            codeStream << " = ";
            generateExpression(varDecl->initializer);
        }
        codeStream << ";\n";
    } else if (auto endLoop = std::dynamic_pointer_cast<ENDLOOP>(stmt)) {
        indent();
        codeStream << "break;\n";
    } else if (auto nextStmt = std::dynamic_pointer_cast<NEXT>(stmt)) {
        indent();
        codeStream << "continue;\n";
    } else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
        indent();
        generateExpression(assign);
        codeStream << ";\n";
    } else if (auto ifStmt = std::dynamic_pointer_cast<If>(stmt)) {
        indent();
        codeStream << "if (";
        generateExpression(ifStmt->condition);
        codeStream << ") {\n";
        indentLevel++;
        for (const auto& s : ifStmt->thenBlock) {
            generateStatement(s);
        }
        indentLevel--;

        // Generate each "else if" block
        for (const auto& elifPair : ifStmt->elifBlocks) {
            indent();
            codeStream << "} else if (";
            generateExpression(elifPair.first);  // Condition for the elif
            codeStream << ") {\n";
            indentLevel++;
            for (const auto& s : elifPair.second) {  // Body of the elif
                generateStatement(s);
            }
            indentLevel--;
        }

        // Generate the else block if it exists
        if (!ifStmt->elseBlock.empty()) {
            indent();
            codeStream << "} else {\n";
            indentLevel++;
            for (const auto& s : ifStmt->elseBlock) {
                generateStatement(s);
            }
            indentLevel--;
        }
        indent();
        codeStream << "}\n";
    } else if (auto whileStmt = std::dynamic_pointer_cast<While>(stmt)) {
        indent();
        codeStream << "while (";
        generateExpression(whileStmt->condition);
        codeStream << ") {\n";
        indentLevel++;
        for (const auto& s : whileStmt->body) {
            generateStatement(s);
        }
        indentLevel--;
        indent();
        codeStream << "}\n";
    } else if (auto forStmt = std::dynamic_pointer_cast<For>(stmt)) {
        indent();
        codeStream << "{\n";
        indentLevel++;

        // Initialize start, end, and step
        indent();
        codeStream << "int __start = ";
        generateExpression(forStmt->start);
        codeStream << ";\n";

        indent();
        codeStream << "int __end = ";
        generateExpression(forStmt->end);
        codeStream << ";\n";

        indent();
        codeStream << "int __step = ";
        if (forStmt->step) {
            codeStream << "(__start > __end ? -(";
            generateExpression(forStmt->step);
            codeStream << ") : (";
            generateExpression(forStmt->step);
            codeStream << "));\n";
            indent();
            codeStream << "if (__step == 0) throw std::runtime_error(\"For loop step cannot be zero.\");\n";
        } else {
            codeStream << "(__start < __end ? 1 : -1);\n";  // Default to 1 or -1 based on start and end
        }

        indent();
        codeStream << "for (int " << forStmt->iterator << " = __start; "
                   << "(__step > 0 ? " << forStmt->iterator << " < __end : " << forStmt->iterator << " > __end); "
                   << forStmt->iterator << " += __step) {\n";
        indentLevel++;
        for (const auto& s : forStmt->body) {
            generateStatement(s);
        }
        indentLevel--;
        indent();
        codeStream << "}\n";

        indentLevel--;
        indent();
        codeStream << "}\n"; // Close the scope
    } else if (auto returnStmt = std::dynamic_pointer_cast<Return>(stmt)) {
        indent();
        codeStream << "return";
        if (returnStmt->value) {
            codeStream << " ";
            generateExpression(returnStmt->value);
        }
        codeStream << ";\n";
    } else if (auto tryCatch = std::dynamic_pointer_cast<TryCatch>(stmt)) {
        indent();
        codeStream << "try {\n";
        indentLevel++;
        for (const auto& s : tryCatch->tryBlock) {
            generateStatement(s);
        }
        indentLevel--;
        indent();
        codeStream << "} catch (std::exception& " << tryCatch->exceptionName << ") {\n";
        indentLevel++;
        for (const auto& s : tryCatch->catchBlock) {
            generateStatement(s);
        }
        indentLevel--;
        indent();
        codeStream << "}\n";
    } else if (auto printStmt = std::dynamic_pointer_cast<Print>(stmt)) {
        indent();
        codeStream << "std::cout << ";
        generateExpression(printStmt->expression);
        codeStream << " << std::endl;\n";
    } else {
        // other statement types
    }
}

void CppCodeGen::generateExpression(const ExprPtr& expr) {
    if (auto literal = std::dynamic_pointer_cast<Literal>(expr)) {
        if (std::holds_alternative<int>(literal->value)) {
            codeStream << std::get<int>(literal->value);
        } else if (std::holds_alternative<float>(literal->value)) {
            codeStream << std::get<float>(literal->value);
        } else if (std::holds_alternative<std::string>(literal->value)) {
            std::string strValue = std::get<std::string>(literal->value);
            if (strValue.size() >= 2 && strValue.front() == '"' && strValue.back() == '"') {
                strValue = strValue.substr(1, strValue.size() - 2);
            }
            codeStream << "\"" << escapeString(strValue) << "\"";
        } else if (std::holds_alternative<bool>(literal->value)) {
            codeStream << (std::get<bool>(literal->value) ? "true" : "false");
        } else if (std::holds_alternative<std::vector<ExprPtr>>(literal->value)) {
            codeStream << "{";
            const auto& elements = std::get<std::vector<ExprPtr>>(literal->value);
            for (size_t i = 0; i < elements.size(); ++i) {
                generateExpression(elements[i]);
                if (i < elements.size() - 1) {
                    codeStream << ", ";
                }
            }
            codeStream << "}";
        }
    } else if (auto ident = std::dynamic_pointer_cast<Identifier>(expr)) {
        // Adjust function names if necessary
        std::string name = ident->name;
        if (name == "main") {
            name = "hl_main";
        }
        codeStream << name;
    } else if (auto binaryOp = std::dynamic_pointer_cast<BinaryOp>(expr)) {
        if (binaryOp->op == BinaryOp::Operator::DIV) {
            codeStream << "safe_divide(";
            generateExpression(binaryOp->left);
            codeStream << ", ";
            generateExpression(binaryOp->right);
            codeStream << ")";
        } else {
            codeStream << "(";
            generateExpression(binaryOp->left);
            codeStream << " " << getOperatorString(binaryOp->op) << " ";
            generateExpression(binaryOp->right);
            codeStream << ")";
        }
    } else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(expr)) {
        if (auto memberAccess = std::dynamic_pointer_cast<MemberAccess>(funcCall->callee)) {
            // Member functions
            if (memberAccess->memberName == "length") {
                generateExpression(memberAccess->object);
                codeStream << ".size()";
            } else if (memberAccess->memberName == "substring") {
                // Generate object (string)
                generateExpression(memberAccess->object);
                codeStream << ".substr(";

                // Generate start index
                generateExpression(funcCall->arguments[0]);
                codeStream << ", ";

                // Generate length (end - start)
                codeStream << "(";
                generateExpression(funcCall->arguments[1]); // end index
                codeStream << " - ";
                generateExpression(funcCall->arguments[0]); // start index
                codeStream << "))";
            } else if (memberAccess->memberName == "concat") {
                generateExpression(memberAccess->object);
                codeStream << " + ";
                generateExpression(funcCall->arguments[0]);
            } else if (memberAccess->memberName == "append") {
                generateExpression(memberAccess->object);
                codeStream << ".push_back(";
                generateExpression(funcCall->arguments[0]);
                codeStream << ")";
            } else if (memberAccess->memberName == "prepend") {
                generateExpression(memberAccess->object);
                codeStream << ".insert(";
                generateExpression(memberAccess->object);
                codeStream << ".begin(), ";
                generateExpression(funcCall->arguments[0]);
                codeStream << ")";
            } else if (memberAccess->memberName == "remove") {
                generateExpression(memberAccess->object);
                codeStream << ".erase(";
                generateExpression(memberAccess->object);
                codeStream << ".begin() + ";
                generateExpression(funcCall->arguments[0]);
                codeStream << ")";
            } else if (memberAccess->memberName == "empty") {
                generateExpression(memberAccess->object);
                codeStream << ".clear()";
            } else {
                // Other member functions
            }
        } else if (auto ident = std::dynamic_pointer_cast<Identifier>(funcCall->callee)) {
            std::string name = ident->name;
            if (name == "main") {
                name = "hl_main";
            }

            if (name == "input") {
                // Generate standard C++ code for input
                codeStream << "([&]() -> std::string { std::string tempInput; std::cout << ";
                generateExpression(funcCall->arguments[0]);
                codeStream << "; std::getline(std::cin, tempInput); return tempInput; })()";
            } else if (name == "INT") {
                codeStream << "std::stoi(";
                generateExpression(funcCall->arguments[0]);
                codeStream << ")";
                return;
            } else if (name == "FLOAT") {
                codeStream << "std::stof(";
                generateExpression(funcCall->arguments[0]);
                codeStream << ")";
                return;
            } else if (name == "STR") {
                codeStream << "std::to_string(";
                generateExpression(funcCall->arguments[0]);
                codeStream << ")";
            } else {
                // Regular function call
                codeStream << name << "(";
                for (size_t i = 0; i < funcCall->arguments.size(); ++i) {
                    generateExpression(funcCall->arguments[i]);
                    if (i < funcCall->arguments.size() - 1) {
                        codeStream << ", ";
                    }
                }
                codeStream << ")";
            }
        } else {
            // Other callee types
            generateExpression(funcCall->callee);
            codeStream << "(";
            for (size_t i = 0; i < funcCall->arguments.size(); ++i) {
                generateExpression(funcCall->arguments[i]);
                if (i < funcCall->arguments.size() - 1) {
                    codeStream << ", ";
                }
            }
            codeStream << ")";
        }
    } else if (auto memberAccess = std::dynamic_pointer_cast<MemberAccess>(expr)) {
        generateExpression(memberAccess->object);
        codeStream << "." << memberAccess->memberName;
    } else if (auto assignment = std::dynamic_pointer_cast<Assignment>(expr)) {
        generateExpression(assignment->target);
        codeStream << " = ";
        generateExpression(assignment->value);
    } else if (auto listAccess = std::dynamic_pointer_cast<ListAccess>(expr)) {
        generateExpression(listAccess->list);
        codeStream << "[";
        generateExpression(listAccess->index);
        codeStream << "]";
    } else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOp>(expr)) {
        codeStream << (unaryOp->op == UnaryOp::Operator::NOT ? "!" : "-");
        generateExpression(unaryOp->operand);
    } else {
        // Other expressions
    }
}

void CppCodeGen::generateType(const TypePtr& type) {
    switch (type->kind) {
        case Type::Kind::INT:
            codeStream << "int";
            break;
        case Type::Kind::FLOAT:
            codeStream << "float";
            break;
        case Type::Kind::STRING:
            codeStream << "std::string";
            break;
        case Type::Kind::BOOL:
            codeStream << "bool";
            break;
        case Type::Kind::LIST:
            codeStream << "std::vector<";
            generateType(type->elementType);
            codeStream << ">";
            break;
        case Type::Kind::VOID:
            codeStream << "void";
            break;
        default:
            codeStream << "auto";
            break;
    }
}

std::string CppCodeGen::getOperatorString(BinaryOp::Operator op) {
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
        case BinaryOp::Operator::AND: return "&&";
        case BinaryOp::Operator::OR:  return "||";
        default: return "";
    }
}

std::string CppCodeGen::escapeString(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '\"': escaped += "\\\""; break;
            case '\n': escaped += "\\n";  break;
            case '\t': escaped += "\\t";  break;
            default:   escaped += c;      break;
        }
    }
    return escaped;
}

void CppCodeGen::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        codeStream << "    ";
    }
}

TypePtr CppCodeGen::inferFunctionReturnType(const std::shared_ptr<Function>& funcDef) {
    for (const auto& stmt : funcDef->body) {
        if (auto returnStmt = std::dynamic_pointer_cast<Return>(stmt)) {
            if (returnStmt->value && returnStmt->value->type) {
                return returnStmt->value->type;
            }
        }
    }
    return std::make_shared<Type>(Type::Kind::VOID);
}

void CppCodeGen::writeToFile(const std::string& filename, const std::string& code) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << code;
        file.close();
    } else {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
}
