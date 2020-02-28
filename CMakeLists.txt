cmake_minimum_required(VERSION 3.15)
project(Neko_Interpreter)

set(CMAKE_CXX_STANDARD 20)

add_executable(Neko_Interpreter main.cpp token.hpp keywords.hpp exceptions.hpp interpreter.hpp helpers.hpp FunctionObject.hpp execution.hpp VariableObject.hpp Container.hpp expressions.hpp ClassObject.hpp Constructions.hpp Values.hpp Object.hpp NekoLib.hpp OperationProcessing.hpp)