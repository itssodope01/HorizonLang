#include "SemanticAnalyzer.h"
#include <unordered_set>

bool SemanticAnalyzer::analyze(const std::shared_ptr<Program>& program) {
    enterScope(); // Start the global scope
    loopDepth = 0;

    currentScope()["Math"] = std::make_shared<Type>(Type::Kind::MATHOBJECT);
    currentInitializedScope()["Math"] = true;


    // First pass: Register all function declarations
    for (const auto& stmt : program->statements) {
        if (auto func = std::dynamic_pointer_cast<Function>(stmt)) {
            registerFunction(func);
        }
    }

    // Second pass: Analyze each statement
    for (const auto& stmt : program->statements) {
        checkStatement(stmt);
    }

    exitScope();
    return errors.empty();
}

const std::vector<std::string>& SemanticAnalyzer::getErrors() const {
    return errors;
}

void SemanticAnalyzer::registerFunction(const std::shared_ptr<Function>& function) {
    if (function->name == "Math") {
        addError("Cannot declare function named 'Math' as it is reserved.");
        return;
    }
    // Check for redeclaration
    if (symbolTableStack.back().find(function->name) != symbolTableStack.back().end()) {
        addError("Function redeclaration: " + function->name);
    } else {
        symbolTableStack.back()[function->name] = nullptr;
        functionReturnTypes[function->name] = nullptr;

        // Store parameter types in functionSignatures
        std::vector<TypePtr> paramTypes;
        for (const auto& param : function->parameters) {
            paramTypes.push_back(param.first);
        }

        functionSignatures[function->name] = paramTypes;
    }
}

void SemanticAnalyzer::addError(const std::string& message) {
    errors.push_back("Error: " + message);
}

TypePtr SemanticAnalyzer::lookupVariable(const std::string& name) {
    for (auto it = symbolTableStack.rbegin(); it != symbolTableStack.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    addError("Undefined variable: " + name);
    return std::make_shared<Type>(Type::Kind::VOID);
}

bool SemanticAnalyzer::isVariableDeclared(const std::string& name) const {
    for (auto it = symbolTableStack.rbegin(); it != symbolTableStack.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

void SemanticAnalyzer::checkStatement(const std::shared_ptr<Statement>& stmt) {
    if (auto tryCatch = std::dynamic_pointer_cast<TryCatch>(stmt)) {
        if (tryCatch->exceptionName == "Math") {
            addError("Exception variable cannot be named 'Math' as it is reserved.");
        }
        // Check try block
        enterScope();
        for (const auto& tryStmt : tryCatch->tryBlock) {
            checkStatement(tryStmt);
        }
        exitScope();

        // Check catch block
        enterScope();
        currentScope()[tryCatch->exceptionName] = std::make_shared<Type>(Type::Kind::VOID);
        currentInitializedScope()[tryCatch->exceptionName] = true;
        for (const auto& catchStmt : tryCatch->catchBlock) {
            checkStatement(catchStmt);
        }
        exitScope();
    } else if (auto varDecl = std::dynamic_pointer_cast<VarDecl>(stmt)) {
        checkVarDeclaration(varDecl);
    } else if (auto func = std::dynamic_pointer_cast<Function>(stmt)) {
        checkFunction(func);
    } else if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt)) {
        if (auto assign = std::dynamic_pointer_cast<Assignment>(exprStmt->expression)) {
            checkAssignment(assign);
        } else {
            checkExpression(exprStmt->expression);
        }
    } else if (auto ifStmt = std::dynamic_pointer_cast<If>(stmt)) {
        checkIfStatement(ifStmt);
    } else if (auto whileStmt = std::dynamic_pointer_cast<While>(stmt)) {
        loopDepth++;
        checkWhileStatement(whileStmt);
        loopDepth--;
    } else if (auto forLoop = std::dynamic_pointer_cast<For>(stmt)) {
        loopDepth++;
        checkForLoop(forLoop);
        loopDepth--;
    } else if (auto terminateStmt = std::dynamic_pointer_cast<ENDLOOP>(stmt)) {
        // Check endloop statement
        if (loopDepth == 0) {
            addError("`endloop` can only be used within a loop.");
        }
    } else if (auto skipitStmt = std::dynamic_pointer_cast<NEXT>(stmt)) {
        // Check next statement
        if (loopDepth == 0) {
            addError("`next` can only be used within a loop.");
        }
    } else if (auto returnStmt = std::dynamic_pointer_cast<Return>(stmt)) {
        checkReturnStatement(returnStmt);
    } else if (auto printStmt = std::dynamic_pointer_cast<Print>(stmt)) {
        checkExpression(printStmt->expression);
    } else if (auto inputStmt = std::dynamic_pointer_cast<Input>(stmt)) {
        TypePtr promptType = checkExpression(inputStmt->prompt);
        if (promptType->kind != Type::Kind::STRING) {
            addError("Input prompt must be a string");
        }
    }
}

TypePtr SemanticAnalyzer::checkExpression(const std::shared_ptr<Expression>& expr) {
    if (!expr) {
        addError("Null expression encountered");
        return std::make_shared<Type>(Type::Kind::VOID);
    }

    if (auto identifier = std::dynamic_pointer_cast<Identifier>(expr)) {
        if (!isVariableInitialized(identifier->name)) {
            addError("Variable '" + identifier->name + "' used before initialization.");
        }
        TypePtr varType = lookupVariable(identifier->name);
        identifier->type = varType; // Assign type to identifier
        return varType;
    }
    else if (auto literal = std::dynamic_pointer_cast<Literal>(expr)) {
        TypePtr litType;
        if (std::holds_alternative<int>(literal->value))
            litType = std::make_shared<Type>(Type::Kind::INT);
        else if (std::holds_alternative<float>(literal->value))
            litType = std::make_shared<Type>(Type::Kind::FLOAT);
        else if (std::holds_alternative<std::string>(literal->value))
            litType = std::make_shared<Type>(Type::Kind::STRING);
        else if (std::holds_alternative<bool>(literal->value))
            litType = std::make_shared<Type>(Type::Kind::BOOL);
        else if (std::holds_alternative<std::vector<ExprPtr>>(literal->value)) {
            // Determine the element types of the list
            const auto& elements = std::get<std::vector<ExprPtr>>(literal->value);
            TypePtr elementType = nullptr;
            bool mixedTypes = false;
            for (const auto& elem : elements) {
                TypePtr elemType = checkExpression(elem);
                if (!elementType) {
                    elementType = elemType;
                } else if (!areTypesCompatible(elementType, elemType)) {
                    mixedTypes = true;
                    break;
                }
            }
            if (mixedTypes) {
                elementType = nullptr; // Indicate mixed types
            }
            litType = std::make_shared<Type>(Type::Kind::LIST, elementType);
        }
        else
            litType = std::make_shared<Type>(Type::Kind::VOID);

        literal->type = litType; // Assign type to literal
        return litType;
    }
    else if (auto binaryOp = std::dynamic_pointer_cast<BinaryOp>(expr)) {
        TypePtr leftType = checkExpression(binaryOp->left);
        TypePtr rightType = checkExpression(binaryOp->right);

        TypePtr resultType;
        if (binaryOp->op == BinaryOp::Operator::ADD) {
            if (leftType->kind == Type::Kind::STRING && rightType->kind == Type::Kind::STRING) {
                resultType = std::make_shared<Type>(Type::Kind::STRING);
            } else {
                resultType = getTypeFromBinaryOp(binaryOp->op, leftType, rightType);
            }
        } else {
            resultType = getTypeFromBinaryOp(binaryOp->op, leftType, rightType);
        }

        binaryOp->type = resultType; // Assign type to binary operation
        return resultType;
    }
    else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOp>(expr)) {
        TypePtr operandType = checkExpression(unaryOp->operand);
        TypePtr resultType = getTypeFromUnaryOp(unaryOp->op, operandType);
        unaryOp->type = resultType; // Assign type to unary operation
        return resultType;
    }
    else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(expr)) {
        TypePtr returnType = checkFunctionCall(funcCall);
        funcCall->type = returnType; // Assign type to function call
        return returnType;
    }
    else if (auto listAccess = std::dynamic_pointer_cast<ListAccess>(expr)) {
        checkListOperation(listAccess);
        TypePtr listType = checkExpression(listAccess->list);
        TypePtr elementType = (listType->kind == Type::Kind::LIST && listType->elementType) ? listType->elementType : std::make_shared<Type>(Type::Kind::VOID);
        listAccess->type = elementType; // Assign type to list access
        return elementType;
    }
    else if (auto memberAccess = std::dynamic_pointer_cast<MemberAccess>(expr)) {
        TypePtr objectType = checkExpression(memberAccess->object);
        TypePtr memberType = nullptr;

        if (objectType->kind == Type::Kind::STRING) {
            if (memberAccess->memberName == "length") {
                memberType = std::make_shared<Type>(Type::Kind::INT);
            } else if (memberAccess->memberName == "substring" || memberAccess->memberName == "concat") {
                memberType = std::make_shared<Type>(Type::Kind::STRING);
            }
        }
        else if (objectType->kind == Type::Kind::LIST) {
            // list member methods
            if (memberAccess->memberName == "append" || memberAccess->memberName == "prepend" || memberAccess->memberName == "remove" || memberAccess->memberName == "empty") {
                memberType = std::make_shared<Type>(Type::Kind::VOID);
            }
        }

        memberAccess->type = memberType;
        return memberType ? memberType : std::make_shared<Type>(Type::Kind::VOID);
    }
    else if (auto assignment = std::dynamic_pointer_cast<Assignment>(expr)) {
        checkAssignment(assignment);
        assignment->type = std::make_shared<Type>(Type::Kind::VOID); // Assign type to assignment
        return assignment->type;
    }

    // Default case: unknown expression type
    expr->type = std::make_shared<Type>(Type::Kind::VOID);
    return expr->type;
}

void SemanticAnalyzer::checkVarDeclaration(const std::shared_ptr<VarDecl>& varDecl) {
    if (varDecl->name == "Math") {
        addError("Cannot declare variable named 'Math' as it is reserved.");
        return;
    }
    if (isVariableDeclared(varDecl->name)) {
        addError("Variable redeclaration: " + varDecl->name);
        return;
    }

    currentScope()[varDecl->name] = varDecl->type;
    currentInitializedScope()[varDecl->name] = varDecl->initializer != nullptr;

    if (varDecl->isConst) {
        constTable[varDecl->name] = varDecl->type;
    }

    if (varDecl->initializer) {
        TypePtr initType = checkExpression(varDecl->initializer);
        if (!areTypesCompatible(varDecl->type, initType)) {
            addError("Type mismatch in variable initialization: " + varDecl->name);
        }
    } else if (varDecl->isConst) {
        addError("Const variable must be initialized: " + varDecl->name);
    }
}

void SemanticAnalyzer::checkAssignment(const std::shared_ptr<Assignment>& assignment) {
    if (auto identifier = std::dynamic_pointer_cast<Identifier>(assignment->target)) {
        // Check if target is a constant
        if (constTable.find(identifier->name) != constTable.end()) {
            addError("Cannot reassign to constant variable: " + identifier->name);
            return;
        }

        // Check if the variable is declared
        if (!isVariableDeclared(identifier->name)) {
            addError("Assignment to undeclared variable: " + identifier->name);
            return;
        }

        TypePtr targetType = lookupVariable(identifier->name);
        TypePtr valueType = checkExpression(assignment->value);

        if (!areTypesCompatible(targetType, valueType)) {
            addError("Type mismatch in assignment to: " + identifier->name);
        }
        currentInitializedScope()[identifier->name] = true;
    } else if (auto listAccess = std::dynamic_pointer_cast<ListAccess>(assignment->target)) {
        checkListOperation(listAccess);
        TypePtr listType = checkExpression(listAccess->list);
        if (listType->kind != Type::Kind::LIST || !listType->elementType) {
            addError("Cannot assign to an element of a non-list type or list with unknown element type.");
            return;
        }
        TypePtr valueType = checkExpression(assignment->value);
        if (!areTypesCompatible(listType->elementType, valueType)) {
            addError("Type mismatch in list assignment.");
        }
    } else {
        addError("Invalid assignment target");
    }

    // Assign type to the assignment expression (void)
    assignment->type = std::make_shared<Type>(Type::Kind::VOID);
}

void SemanticAnalyzer::checkFunction(const std::shared_ptr<Function>& function) {
    enterScope();
    currentFunctionName = function->name;
    currentFunctionReturnType = std::nullopt; // Return type will be inferred
    hasReturnStatement = false;

    if (function->name == "Math") {
        addError("Cannot declare function named 'Math' as it is reserved.");
        return;
    }

    // Check parameter types and names
    std::unordered_set<std::string> paramNames;
    for (const auto& param : function->parameters) {
        if (!paramNames.insert(param.second).second) {
            addError("Duplicate parameter name in function " + function->name + ": " + param.second);
        }
        currentScope()[param.second] = param.first;
        currentInitializedScope()[param.second] = true;
    }

    // Process all statements in the function body
    for (const auto& stmt : function->body) {
        checkStatement(stmt);
    }

    if (!currentFunctionReturnType.has_value()) {
        // If no return statement encountered, set return type to void
        currentFunctionReturnType = std::make_shared<Type>(Type::Kind::VOID);
    }

    // Update functionReturnTypes and symbol table with the determined return type
    functionReturnTypes[function->name] = currentFunctionReturnType.value();
    symbolTableStack.front()[function->name] = currentFunctionReturnType.value();

    currentFunctionReturnType = std::nullopt;
    currentFunctionName = "";
    exitScope();
}


void SemanticAnalyzer::checkReturnStatement(const std::shared_ptr<Return>& returnStmt) {
    if (currentFunctionName.empty()) {
        addError("Return statement outside of function");
        return;
    }

    hasReturnStatement = true;

    TypePtr returnType;
    if (returnStmt->value) {
        returnType = checkExpression(returnStmt->value);
    } else {
        returnType = std::make_shared<Type>(Type::Kind::VOID);
    }

    if (!currentFunctionReturnType.has_value()) {
        // First return statement; set function return type
        currentFunctionReturnType = returnType;

        // Update function return type in functionReturnTypes and symbol table
        functionReturnTypes[currentFunctionName] = returnType;
        symbolTableStack.front()[currentFunctionName] = returnType;
    } else {
        TypePtr expectedReturnType = currentFunctionReturnType.value();

        if (!areTypesCompatible(expectedReturnType, returnType)) {
            addError("Return type mismatch in function '" + currentFunctionName +
                     "': expected " + ASTPrinter::typeKindToString(expectedReturnType->kind) +
                     ", got " + ASTPrinter::typeKindToString(returnType->kind));
        }
    }
}


TypePtr SemanticAnalyzer::checkFunctionCall(const std::shared_ptr<FunctionCall>& funcCall) {
    if (auto identifier = std::dynamic_pointer_cast<Identifier>(funcCall->callee)) {
        if (identifier->name == "input") {
            if (funcCall->arguments.size() != 1) {
                addError("Input function expects exactly one argument.");
            }
            TypePtr argType = checkExpression(funcCall->arguments[0]);
            if (argType->kind != Type::Kind::STRING) {
                addError("Input prompt must be a string");
            }
            return std::make_shared<Type>(Type::Kind::STRING);
        }

        // handling for 'STR'
        if (identifier->name == "STR") {
            if (funcCall->arguments.size() != 1) {
                addError("Function `STR` expects exactly one argument.");
                return std::make_shared<Type>(Type::Kind::VOID);
            }
            TypePtr argType = checkExpression(funcCall->arguments[0]);

            if (argType->kind == Type::Kind::VOID) {
                if (std::dynamic_pointer_cast<ListAccess>(funcCall->arguments[0])) {
                    // Allow calling STR on unknown types from list access
                } else {
                    addError("`STR` function argument must be int, float, bool, or string.");
                }
            } else if (argType->kind != Type::Kind::INT &&
                       argType->kind != Type::Kind::FLOAT &&
                       argType->kind != Type::Kind::BOOL &&
                       argType->kind != Type::Kind::STRING) {
                addError("`STR` function argument must be int, float, bool, or string.");
            }
            return std::make_shared<Type>(Type::Kind::STRING);
        }

        // handling for 'int'
        if (identifier->name == "INT") {
            if (funcCall->arguments.size() != 1) {
                addError("Function `INT` expects exactly one argument.");
                return std::make_shared<Type>(Type::Kind::INT);
            }
            TypePtr argType = checkExpression(funcCall->arguments[0]);

            if (argType->kind == Type::Kind::VOID) {
                if (!std::dynamic_pointer_cast<ListAccess>(funcCall->arguments[0])) {
                    addError("`INT` function argument must be float, bool, or string.");
                }
            } else if (argType->kind != Type::Kind::FLOAT &&
                       argType->kind != Type::Kind::BOOL &&
                       argType->kind != Type::Kind::STRING
                       ) {
                addError("`INT` function argument must be float, bool, or string.");
            }
            return std::make_shared<Type>(Type::Kind::INT);
        }

        // handling for 'float'
        if (identifier->name == "FLOAT") {
            if (funcCall->arguments.size() != 1) {
                addError("Function `FLOAT` expects exactly one argument.");
                return std::make_shared<Type>(Type::Kind::FLOAT);
            }
            TypePtr argType = checkExpression(funcCall->arguments[0]);

            if (argType->kind == Type::Kind::VOID) {
                if (!std::dynamic_pointer_cast<ListAccess>(funcCall->arguments[0])) {
                    addError("`FLOAT` function argument must be int, bool, or string.");
                }
            } else if (argType->kind != Type::Kind::INT &&
                       argType->kind != Type::Kind::BOOL &&
                       argType->kind != Type::Kind::STRING) {
                addError("`FLOAT` function argument must be int, bool, or string.");
            }
            return std::make_shared<Type>(Type::Kind::FLOAT);
        }

        if (!isVariableDeclared(identifier->name)) {
            addError("Call to undeclared function: " + identifier->name);
            return std::make_shared<Type>(Type::Kind::VOID);
        }

        // Retrieve function signature
        auto it = functionSignatures.find(identifier->name);
        if (it == functionSignatures.end()) {
            addError("Function signature not found: " + identifier->name);
            return std::make_shared<Type>(Type::Kind::VOID);
        }

        const auto& paramTypes = it->second;
        if (funcCall->arguments.size() != paramTypes.size()) {
            addError("Incorrect number of arguments for function: " + identifier->name);
            return std::make_shared<Type>(Type::Kind::VOID);
        }

        // Check each argument type
        for (size_t i = 0; i < paramTypes.size(); ++i) {
            TypePtr argType = checkExpression(funcCall->arguments[i]);
            if (!areTypesCompatible(paramTypes[i], argType)) {
                addError("Argument type mismatch in function call to \"" + identifier->name + "\"" + " at parameter " + std::to_string(i + 1));
            }
        }

        auto returnTypeIt = functionReturnTypes.find(identifier->name);
        if (returnTypeIt != functionReturnTypes.end() && returnTypeIt->second) {
            return returnTypeIt->second;
        } else {
            addError("Function return type not determined for: " + identifier->name);
            return std::make_shared<Type>(Type::Kind::VOID);
        }
    }

    // Handle member methods
    if (auto memberAccess = std::dynamic_pointer_cast<MemberAccess>(funcCall->callee)) {
        TypePtr objectType = checkExpression(memberAccess->object);

        // Check if the method is called on a list
        if (objectType->kind == Type::Kind::LIST) {
            std::string methodName = memberAccess->memberName;

            if (methodName == "append" || methodName == "prepend") {
                // `append` and `prepend` require exactly one argument
                if (funcCall->arguments.size() != 1) {
                    addError("Method `" + methodName + "` requires exactly one argument.");
                } else {
                    TypePtr argType = checkExpression(funcCall->arguments[0]);
                    TypePtr elementType = objectType->elementType;

                    // Check if list is not a mixed list
                    if (elementType && !areTypesCompatible(elementType, argType)) {
                        addError("Argument type mismatch for method `" + methodName + "`.");
                    }
                }
                return std::make_shared<Type>(Type::Kind::VOID);
            }
            else if (methodName == "remove") {
                // `remove` requires exactly one argument
                if (funcCall->arguments.size() != 1) {
                    addError("Method `remove` requires exactly one argument.");
                } else {
                    TypePtr argType = checkExpression(funcCall->arguments[0]);
                    if (argType->kind != Type::Kind::INT) {
                        addError("Argument to `remove` must be an integer index.");
                    }
                }
                return std::make_shared<Type>(Type::Kind::VOID);
            }
            else if (methodName == "empty") {
                // `empty` should not accept any arguments
                if (!funcCall->arguments.empty()) {
                    addError("Method `empty` does not accept any arguments.");
                }
                return std::make_shared<Type>(Type::Kind::VOID);
            } else if (methodName == "length") {
                // `length` should not accept any arguments
                if (!funcCall->arguments.empty()) {
                    addError("Method `length` expects no arguments.");
                }
                return std::make_shared<Type>(Type::Kind::INT);
            }
            else {
                addError("Invalid method `" + methodName + "` for list type.");
            }
        }

        if (objectType->kind == Type::Kind::STRING) {
            std::string methodName = memberAccess->memberName;

            if (methodName == "length") {
                // `length` should not accept any arguments
                if (!funcCall->arguments.empty()) {
                    addError("Method `length` expects no arguments.");
                }
                return std::make_shared<Type>(Type::Kind::INT);
            } else if (methodName == "substring") {
                // `substring` requires exactly two integer arguments
                if (funcCall->arguments.size() != 2) {
                    addError("Method `substring` expects two integer arguments.");
                } else {
                    TypePtr arg1Type = checkExpression(funcCall->arguments[0]);
                    TypePtr arg2Type = checkExpression(funcCall->arguments[1]);
                    if (arg1Type->kind != Type::Kind::INT || arg2Type->kind != Type::Kind::INT) {
                        addError("Arguments to `substring` must be integers.");
                    }
                }
                return std::make_shared<Type>(Type::Kind::STRING);
            } else if (methodName == "concat") {
                // `concat` requires exactly one string argument
                if (funcCall->arguments.size() != 1) {
                    addError("Method `concat` expects one string argument.");
                } else {
                    TypePtr argType = checkExpression(funcCall->arguments[0]);
                    if (argType->kind != Type::Kind::STRING) {
                        addError("Argument to `concat` must be a string.");
                    }
                }
                return std::make_shared<Type>(Type::Kind::STRING);
            } else if (methodName == "toUpper" || methodName == "toLower") {
                // should not accept any arguments
                if (!funcCall->arguments.empty()) {
                    addError("Method `"+ methodName + "` expects no arguments.");
                }
                return std::make_shared<Type>(Type::Kind::STRING);
            } else if (methodName == "sub") {
                // should not accept any arguments
                if (funcCall->arguments.size() != 2) {
                    addError("Method `"+ methodName + "` expects two string arguments.");
                } else {
                    TypePtr argType1 = checkExpression(funcCall->arguments[0]);
                    TypePtr argType2 = checkExpression(funcCall->arguments[1]);
                    if (argType1->kind != Type::Kind::STRING || argType2->kind != Type::Kind::STRING) {
                        addError("Arguments to `sub` must be strings.");
                    }
                }
                return std::make_shared<Type>(Type::Kind::STRING);
            } else {
                addError("Invalid method `" + methodName + "` for string type.");
                return std::make_shared<Type>(Type::Kind::VOID);
            }
        }
        if (objectType->kind == Type::Kind::INT) {
            std::string methodName = memberAccess->memberName;

            if (methodName == "power") {
            // `concat` requires exactly one string argument
                if (funcCall->arguments.size() != 1) {
                    addError("Method `power` expects one integer argument.");
                } else {
                    TypePtr argType = checkExpression(funcCall->arguments[0]);
                    if (argType->kind != Type::Kind::INT) {
                        addError("Argument to `power` must be integer.");
                    }
                }
                return std::make_shared<Type>(Type::Kind::INT);
            }
        }
        if (objectType->kind == Type::Kind::MATHOBJECT) {
            std::string methodName = memberAccess->memberName;
            if (methodName == "power") {
                // 'power' requires exactly two numeric arguments
                if (funcCall->arguments.size() != 2) {
                    addError("Method 'power' expects two numeric arguments.");
                } else {
                    TypePtr arg1Type = checkExpression(funcCall->arguments[0]);
                    TypePtr arg2Type = checkExpression(funcCall->arguments[1]);
                    if (!isNumericType(arg1Type) || !isNumericType(arg2Type)) {
                        addError("Arguments to 'power' must be numeric.");
                    }
                }
                // Determine return type based on argument types
                if (funcCall->arguments.size() == 2) {
                    TypePtr arg1Type = funcCall->arguments[0]->type;
                    TypePtr arg2Type = funcCall->arguments[1]->type;
                    if (arg1Type->kind == Type::Kind::INT && arg2Type->kind == Type::Kind::INT) {
                        return std::make_shared<Type>(Type::Kind::INT);
                    } else {
                        return std::make_shared<Type>(Type::Kind::FLOAT);
                    }
                } else {
                    return std::make_shared<Type>(Type::Kind::VOID);
                }
            }
            else if (methodName == "sqrt") {
                // 'sqrt' requires exactly one numeric argument
                if (funcCall->arguments.size() != 1) {
                    addError("Method 'sqrt' expects one numeric argument.");
                } else {
                    TypePtr argType = checkExpression(funcCall->arguments[0]);
                    if (!isNumericType(argType)) {
                        addError("Argument to 'sqrt' must be numeric.");
                    }
                }
                return std::make_shared<Type>(Type::Kind::FLOAT);
            }
            else if (methodName == "abs") {
                // 'abs' requires exactly one numeric argument
                if (funcCall->arguments.size() != 1) {
                    addError("Method 'abs' expects one numeric argument.");
                } else {
                    TypePtr argType = checkExpression(funcCall->arguments[0]);
                    if (!isNumericType(argType)) {
                        addError("Argument to 'abs' must be numeric.");
                    }
                }
                // Return type depends on argument type
                if (funcCall->arguments.size() == 1) {
                    TypePtr argType = funcCall->arguments[0]->type;
                    return std::make_shared<Type>(argType->kind);
                } else {
                    return std::make_shared<Type>(Type::Kind::VOID);
                }
            }
            else if (methodName == "round") {
                // 'round' requires exactly one numeric argument
                if (funcCall->arguments.size() != 1) {
                    addError("Method 'round' expects one numeric argument.");
                } else {
                    TypePtr argType = checkExpression(funcCall->arguments[0]);
                    if (!isNumericType(argType)) {
                        addError("Argument to 'round' must be numeric.");
                    }
                }
                 return std::make_shared<Type>(Type::Kind::INT);
            }
            else {
                addError("Invalid method '" + methodName + "' for Math object.");
            }
        }

    }

    addError("Invalid function call target");
    return std::make_shared<Type>(Type::Kind::VOID);
}

void SemanticAnalyzer::checkListOperation(const std::shared_ptr<ListAccess>& listOp) {
    TypePtr listType = checkExpression(listOp->list);
    TypePtr indexType = checkExpression(listOp->index);

    if (listType->kind != Type::Kind::LIST) {
        addError("Attempt to index a non-list type");
    }

    if (indexType->kind != Type::Kind::INT) {
        addError("List index must be an integer");
    }

    // Assign type to list access
    listOp->type = (listType->kind == Type::Kind::LIST && listType->elementType) ? listType->elementType : std::make_shared<Type>(Type::Kind::VOID);
}

void SemanticAnalyzer::checkForLoop(const std::shared_ptr<For>& forLoop) {
    enterScope();

    if (forLoop->iterator == "Math") {
        addError("Loop iterator cannot be named 'Math' as it is reserved.");
    }

    // Mark the iterator as an initialized integer
    currentScope()[forLoop->iterator] = std::make_shared<Type>(Type::Kind::INT);
    currentInitializedScope()[forLoop->iterator] = true;

    TypePtr startType = checkExpression(forLoop->start);
    TypePtr endType = checkExpression(forLoop->end);

    if (startType->kind != Type::Kind::INT || endType->kind != Type::Kind::INT) {
        addError("For loop range must be integers");
    }

    if (forLoop->step) {
        TypePtr stepType = checkExpression(forLoop->step);
        if (stepType->kind != Type::Kind::INT) {
            addError("For loop step must be an integer");
        }

        // Check if step is a literal and verify it's positive
        if (auto stepLiteral = std::dynamic_pointer_cast<Literal>(forLoop->step)) {
            if (std::holds_alternative<int>(stepLiteral->value)) {
                int stepValue = std::get<int>(stepLiteral->value);
                if (stepValue <= 0) {
                    addError("For loop step increment must be positive");
                }
            }
        } else {
            addError("For loop step must be a positive literal integer");
        }
    }

    for (const auto& stmt : forLoop->body) {
        checkStatement(stmt);
    }

    exitScope();
}

void SemanticAnalyzer::checkIfStatement(const std::shared_ptr<If>& ifStmt) {
    TypePtr condType = checkExpression(ifStmt->condition);
    if (condType->kind != Type::Kind::BOOL) {
        addError("If condition must be a boolean expression");
    }

    enterScope();
    for (const auto& stmt : ifStmt->thenBlock) {
        checkStatement(stmt);
    }
    exitScope();

    for (const auto& elif : ifStmt->elifBlocks) {
        TypePtr elifCondType = checkExpression(elif.first);
        if (elifCondType->kind != Type::Kind::BOOL) {
            addError("Elif condition must be a boolean expression");
        }

        enterScope();
        for (const auto& stmt : elif.second) {
            checkStatement(stmt);
        }
        exitScope();
    }

    enterScope();
    for (const auto& stmt : ifStmt->elseBlock) {
        checkStatement(stmt);
    }
    exitScope();
}

void SemanticAnalyzer::checkWhileStatement(const std::shared_ptr<While>& whileStmt) {
    TypePtr condType = checkExpression(whileStmt->condition);
    if (condType->kind != Type::Kind::BOOL) {
        addError("While condition must be a boolean expression");
    }

    enterScope();
    for (const auto& stmt : whileStmt->body) {
        checkStatement(stmt);
    }
    exitScope();
}

bool SemanticAnalyzer::areTypesCompatible(TypePtr expected, TypePtr actual) const {
    if (!expected || !actual) {
        return false;
    }

    if (expected->kind == actual->kind) {
        if (expected->kind == Type::Kind::LIST) {
            if (expected->elementType && actual->elementType) {
                return areTypesCompatible(expected->elementType, actual->elementType);
            }
            return true;
        }
        return true;
    }
    // Allow implicit conversion from INT to FLOAT
    if (expected->kind == Type::Kind::FLOAT && actual->kind == Type::Kind::INT) {
        return true;
    }
    return false;
}


TypePtr SemanticAnalyzer::getTypeFromBinaryOp(BinaryOp::Operator op, TypePtr left, TypePtr right) {
    switch (op) {
        case BinaryOp::Operator::ADD:
        case BinaryOp::Operator::SUB:
        case BinaryOp::Operator::MUL:
        case BinaryOp::Operator::DIV:
            if (!isNumericType(left) || !isNumericType(right)) {
                addError("Arithmetic operations require numeric operands");
                return std::make_shared<Type>(Type::Kind::VOID);
            }
            // If either operand is float, result is float
            if (left->kind == Type::Kind::FLOAT || right->kind == Type::Kind::FLOAT) {
                return std::make_shared<Type>(Type::Kind::FLOAT);
            } else {
                return std::make_shared<Type>(Type::Kind::INT);
            }

        case BinaryOp::Operator::MOD:
            if (left->kind != Type::Kind::INT || right->kind != Type::Kind::INT) {
                addError("Modulo operation requires integer operands");
                return std::make_shared<Type>(Type::Kind::VOID);
            }
            return std::make_shared<Type>(Type::Kind::INT);

        case BinaryOp::Operator::EQ:
        case BinaryOp::Operator::NE:
        case BinaryOp::Operator::LT:
        case BinaryOp::Operator::GT:
        case BinaryOp::Operator::LE:
        case BinaryOp::Operator::GE:
            if (!areTypesCompatible(left, right) && !areTypesCompatible(right, left)) {
                addError("Comparison operators require compatible types");
                return std::make_shared<Type>(Type::Kind::VOID);
            }
            return std::make_shared<Type>(Type::Kind::BOOL);

        case BinaryOp::Operator::AND:
        case BinaryOp::Operator::OR:
            if (left->kind != Type::Kind::BOOL || right->kind != Type::Kind::BOOL) {
                addError("Logical operators require boolean operands");
                return std::make_shared<Type>(Type::Kind::VOID);
            }
            return std::make_shared<Type>(Type::Kind::BOOL);

        default:
            addError("Unknown binary operator");
            return std::make_shared<Type>(Type::Kind::VOID);
    }
}

TypePtr SemanticAnalyzer::getTypeFromUnaryOp(UnaryOp::Operator op, TypePtr operand) {
    switch (op) {
        case UnaryOp::Operator::NOT:
            if (operand->kind != Type::Kind::BOOL) {
                addError("Logical NOT operator requires boolean operand");
                return std::make_shared<Type>(Type::Kind::VOID);
            }
            return std::make_shared<Type>(Type::Kind::BOOL);

        case UnaryOp::Operator::MINUS:
            if (!isNumericType(operand)) {
                addError("Unary minus requires numeric operand");
                return std::make_shared<Type>(Type::Kind::VOID);
            }
            return operand;

        default:
            addError("Unknown unary operator");
            return std::make_shared<Type>(Type::Kind::VOID);
    }
}

bool SemanticAnalyzer::isNumericType(const TypePtr& type) {
    return type->kind == Type::Kind::INT || type->kind == Type::Kind::FLOAT;
}

void SemanticAnalyzer::enterScope() {
    symbolTableStack.emplace_back();
    variableInitializedStack.emplace_back();
}

void SemanticAnalyzer::exitScope() {
    if (!symbolTableStack.empty()) {
        symbolTableStack.pop_back();
    }
    if (!variableInitializedStack.empty()) {
        variableInitializedStack.pop_back();
    }
}

std::unordered_map<std::string, TypePtr>& SemanticAnalyzer::currentScope() {
    if (symbolTableStack.empty()) {
        addError("Internal error: No active scope");
        symbolTableStack.emplace_back();
    }
    return symbolTableStack.back();
}

std::unordered_map<std::string, bool>& SemanticAnalyzer::currentInitializedScope() {
    if (variableInitializedStack.empty()) {
        addError("Internal error: No active initialized scope");
        variableInitializedStack.emplace_back();
    }
    return variableInitializedStack.back();
}

bool SemanticAnalyzer::isVariableInitialized(const std::string& name) const {
    for (auto it = variableInitializedStack.rbegin(); it != variableInitializedStack.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    return false; // Variable not found or not initialized
}