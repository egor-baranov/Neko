#ifndef NEKO_INTERPRETER_FUNCTIONOBJECT_HPP
#define NEKO_INTERPRETER_FUNCTIONOBJECT_HPP

#include "VariableObject.hpp"
#include "expressions.hpp"
#include "NekoLib.hpp"

bool operator>(Function a, Function b) {
	return a.name > b.name;
}

bool operator<(Function a, Function b) {
	return a.name < b.name;
}

bool operator==(Function a, Function b) {
	return a.name == b.name;
}

Exception parseFunctionDeclaration(const vector<Token> &input, int &index) {
	if (input.size() - index < 6) {
		return Exception(FunctionDeclarationError, getLineIndex(input, index));
	}
	Function functionObject = Function();
	functionObject.startIndex = getLineIndex(input, index);
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
	index = nextIndex(input, index);
	// Аргументы функции
	while (input[index].source != ")") {
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
			functionArgument.type = input[index].source;
			index = nextIndex(input, index);
		}
		if (not contain({")", ","}, input[index].source)) {
			return Exception(FunctionDeclarationError, getLineIndex(input, index));
		}
		functionObject.args.push_back(functionArgument);
		if (input[index].source == ",") {
			index = nextIndex(input, index);
		}
	}
	index = nextIndex(input, index);
	// тип функции
	if (input[index].source == ":") {
		index = nextIndex(input, index);
		if (input[index].type != Name) {
			return Exception(FunctionDeclarationError, getLineIndex(input, index));
		}
		if (not isBuildInType(input[index]) and not isDeclaredClass(input[index])) {
			return Exception(UnknownTypeError, getLineIndex(input, index));
		}
		functionObject.type = input[index].source;
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
		// functionObject.representation.push_back(result.source);
	} else {
		index = nextIndex(input, index);
		stack<string> bracketStack;
		while (!(bracketStack.empty() and input[index].source == "}")) {
			if (input[index].source == "{") {
				bracketStack.push(input[index].source);
			}
			if (input[index].source == "}") {
				bracketStack.pop();
			}
			functionObject.representation.push_back(input[index]);
			index = nextIndex(input, index);
		}
		index = nextIndex(input, index);
	}
	Functions[functionObject.name] = functionObject;
	return Exception(Nothing);
}

FunctionReturned call(string functionName, vector<Item> &args, int functionCallIndex) {
	if (Functions.find(functionName) != Functions.end()) {
		return Functions[functionName].runWithArgs(args);
	} else if (BuiltInFunctions.find(functionName) != BuiltInFunctions.end()) {
		return callBuiltInFunction(functionName, args);
	} else {
		return Exception(UndefinedNameUsage, functionCallIndex);
	}
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

// TODO: add returned value and argument check

#endif //NEKO_INTERPRETER_FUNCTIONOBJECT_HPP
