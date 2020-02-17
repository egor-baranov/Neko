cmake_minimum_required(VERSION 3.15)
project(Neko_Interpreter)

set(CMAKE_CXX_STANDARD 20)

add_executable(Neko_Interpreter main.cpp token.h keywords.h exceptions.h interpreter.h helpers.h FunctionObject.h execution.h VariableObject.h Container.h expressions.h ClassObject.h Constructions.h Values.h)