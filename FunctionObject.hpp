#ifndef NEKO_INTERPRETER_FUNCTIONOBJECT_HPP
#define NEKO_INTERPRETER_FUNCTIONOBJECT_HPP

#include "VariableObject.hpp"
#include "expressions.hpp"
#include "NekoLib.hpp"

bool operator>(FunctionObject a, FunctionObject b) {
	return a.name > b.name;
}

bool operator<(FunctionObject a, FunctionObject b) {
	return a.name < b.name;
}

bool operator==(FunctionObject a, FunctionObject b) {
	return a.name == b.name;
}

Exception parseFunctionDeclaration(const vector<Token> &input, int &index) {
	Namespace nameSpace = InsideOfFunction;
	if (input.size() < 6) {
		return Exception(FunctionDeclarationError, getLineIndex(input, index));
	}
	FunctionObject functionObject = FunctionObject();
	if (not contain({"fun", "lambda"}, input[index].source)) {
		return Exception(FunctionDeclarationError, getLineIndex(input, index));
	}
	// есть ли имя у функции
	functionObject.isLambda = input[index].source == "lambda";
	if (input[index].source == "fun") {
		index = nextIndex(input, index);
		if (input[index].type != Name) {
			return Exception(FunctionDeclarationError, getLineIndex(input, index));
		}
		functionObject.name = input[index].source;
	}
	index = nextIndex(input, index);
	if (input[index].source != "(") {
		return Exception(FunctionDeclarationError, getLineIndex(input, index));
	}
	// Аргументы функции
	while (input[index].source != ")") {
		index = nextIndex(input, index);
		FunctionArgument functionArgument;
		if (input[index].source == "ref") {
			functionArgument.ref = true;
			index = nextIndex(input, index);
		}
		if (input[index].type != Name) {
			return Exception(FunctionDeclarationError, getLineIndex(input, index));
		}
		functionArgument.name = input[index].source;
		index = nextIndex(input, index);
		if (input[index].source == ":") {
			index = nextIndex(input, index);
			if (input[index].type != Name) {
				return Exception(FunctionDeclarationError, getLineIndex(input, index));
			}
			if (not isBuildInType(input[index]) and not isDeclaredClass(input[index])) {
				return Exception(UnknownTypeError, getLineIndex(input, index));
			}
			functionArgument.typeName = input[index].source;
			index = nextIndex(input, index);
		}
		if (not contain({")", ","}, input[index].source)) {
			return Exception(FunctionDeclarationError, getLineIndex(input, index));
		}
		functionObject.args.push_back(functionArgument);
	}
	index = nextIndex(input, index);
	// тип функции
	if (input[index].source == ":") {
		index = nextIndex(input, index);
		if (input[index].type != Name) {
			return Exception(FunctionDeclarationError, getLineIndex(input, index));
		}
		functionObject.typeName = input[index].source;
		index = nextIndex(input, index);
	}
	if (not contain({"=", "{"}, input[index].source)) {
		return Exception(FunctionDeclarationError, getLineIndex(input, index));
	}
	if (input[index].source == "=") {
		index = nextIndex(input, index);
		ParseExpressionReturned result = parseExpression(input, index);
		if (result.exception.type != Nothing) {
			return result.exception;
		}
		functionObject.representation.push_back(result.source);
	} else {
		index = nextIndex(input, index);
		while (input[index].source != "}") {
			ParseExpressionReturned result = parseExpression(input, index);
			if (result.exception.type != Nothing) {
				return result.exception;
			}
			functionObject.representation.push_back(result.source);
		}
	}
	Functions[functionObject.name] = functionObject;
	return Exception(Nothing);
}

// TODO: добавить возвращаемое значение
FunctionReturned call(string functionName, vector<Item> &args, int functionCallIndex) {
	if (Functions.find(functionName) != Functions.end()) {
		Functions[functionName].runWithArgs(args);
	} else if (BuiltInFunctions.find(functionName) != BuiltInFunctions.end()) {
		return callBuiltInFunction(functionName, args);
	} else {
		return Exception(UndefinedNameUsage, functionCallIndex);
	}
	return Exception(Nothing);
}

FunctionReturned parseFunctionCall(const vector<Token> &input, int &index) {
	string functionName = input[index].source;
	int functionCallIndex = getLineIndex(input, index);
	index = nextIndex(input, index);
	if (input[index].source != "(") {
		return Exception(FunctionCallError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	vector<Item> args;
	while (input[index].source != ")") {
		auto result = parseExpression(input, index);
		if (result.exception.type != Nothing) {
			return Exception(result.exception.type, getLineIndex(input, index));
		}
		CalculateReturned calculateReturned = Calculate(result.source);
		if (calculateReturned.exception.type != Nothing) {
			return Exception(calculateReturned.exception.type, getLineIndex(input, index));
		}
		args.push_back(calculateReturned.item);
		if (not contain({")", ","}, input[index].source)) {
			return Exception(FunctionCallError, getLineIndex(input, index));
		}
		if (input[index].source == ",") {
			index = nextIndex(input, index);
		}
	}
	index = nextIndex(input, index);
	return call(functionName, args, functionCallIndex);
}

#endif //NEKO_INTERPRETER_FUNCTIONOBJECT_HPP
