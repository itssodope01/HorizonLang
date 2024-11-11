cmake_minimum_required(VERSION 3.29)
project(HorizonLang)

set(CMAKE_CXX_STANDARD 20)

add_executable(HorizonLang main.cpp
        lexer/Token.h
        lexer/Lexer.h
        lexer/Lexer.cpp
        lexer/Token_Utils.h)
