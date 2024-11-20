cmake_minimum_required(VERSION 3.29)
project(HorizonLang)

set(CMAKE_CXX_STANDARD 20)

add_executable(HorizonLang main.cpp
        lexer/Token.h
        lexer/Lexer.h
        lexer/Lexer.cpp
        lexer/Token_Utils.h
        ast/AST.h
        parser/Parser.h
        parser/Parser.cpp
        ast/ASTprint.cpp
        ast/ASTprint.h
        semantic/SemanticAnalyzer.h
        semantic/SemanticAnalyzer.cpp
        codegen/Python/PythonCodegen.h
        codegen/Python/PythonCodegen.cpp
        codegen/CPP/CppCodeGen.h
        codegen/CPP/CppCodeGen.cpp
)
add_executable(output_executable output.cpp)

