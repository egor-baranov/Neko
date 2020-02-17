#ifndef NEKO_INTERPRETER_EXECUTION_H
#define NEKO_INTERPRETER_EXECUTION_H

#include "token.h"
#include "FunctionObject.h"
#include "VariableObject.h"
#include "ClassObject.h"

enum nameType {
  DeclaredVariable,
  DeclaredFunction,
  DeclaredClass,
  Undeclared
};

nameType nameDeclaration(string name) {
	if (Variables.find(name) != Variables.end()) {
		return DeclaredVariable;
	}
	if (Functions.find(name) != Functions.end()) {
		return DeclaredFunction;
	}
	if (Classes.find(name) != Classes.end()) {

	}
	return Undeclared;
}

Exception execute(Expression expression) {

	return Exception(Nothing);
}

Exception execute(vector<Token> input) {
	int index = 0;
	int end = input.size() - 1;
	while (index < end) {
		Token token = input[index];
		Token prevToken = prev(input, index), nextToken = next(input, index);
		if (token.type == EOL) {
			++index;
			continue;
		}
		if (contain({"val", "var"}, token.source) or
		    (token.type == Name and nameDeclaration(token.source) == Undeclared)) {
			Exception exception = parseVariableDeclaration(input, index);
			if (exception.type == Nothing) continue;
			return exception;
		}
		if (token.source == "fun") {
			Exception exception = parseFunctionDeclaration(input, index);
			if (exception.type == Nothing) continue;
			return exception;
		}
		if (token.source == "class") {
			Exception exception = parseClassDeclaration(input, index);
			if (exception.type == Nothing) continue;
			return exception;
		}
		auto returnedExpression = parseExpression(input, index);
		if (returnedExpression.exception.type != Nothing) {
			return returnedExpression.exception;
		}
		execute(returnedExpression.source);
	}
}

#endif //NEKO_INTERPRETER_EXECUTION_H
