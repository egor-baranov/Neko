#ifndef NEKO_INTERPRETER_EXECUTION_HPP
#define NEKO_INTERPRETER_EXECUTION_HPP

#include "token.hpp"
#include "FunctionObject.hpp"
#include "VariableObject.hpp"
#include "ClassObject.hpp"
#include "Constructions.hpp"

Exception parseIfStatement(const vector<Token> &input, int &index);

// TODO: FIX short expression
Exception execute(vector<Token> input) {
	int index = 0;
	if (input.back().type != EOfF) {
		input.push_back(endOfFile);
	}
	int end = input.size() - 1;
	while (index < end) {
		Token token = input[index];
		Token prevToken = prev(input, index), nextToken = next(input, index);
		if (token.type == EOE or token.type == EOL) {
			++index;
			continue;
		}
		if (contain({"val", "var"}, token.source) or
		    (token.type == Name and nameDeclaration(token.source) == Undeclared and nextToken.source == "=")) {
			Exception exception = parseVariableDeclaration(input, index);
			if (exception.type == Nothing) continue;
			return Exception(exception.type, getLineIndex(input, index));
		}
		if (token.source == "fun") {
			Exception exception = parseFunctionDeclaration(input, index);
			if (exception.type == Nothing) continue;
			return Exception(exception.type, getLineIndex(input, index));
		}
		if (token.source == "class") {
			Exception exception = parseClassDeclaration(input, index);
			if (exception.type == Nothing) continue;
			return Exception(exception.type, getLineIndex(input, index));
		}
		if (nameDeclaration(token.source) == DeclaredVariable and nextToken.type == AssignmentOperator) {
			Exception exception = parseVariableAssignment(input, index).exception;
			if (exception.type == Nothing) continue;
			return Exception(exception.type, getLineIndex(input, index));
		}
		if (nameDeclaration(token.source) == DeclaredFunction) {
			Exception exception = parseFunctionCall(input, index).exception;
			if (exception.type == Nothing) continue;
			return Exception(exception.type, getLineIndex(input, index));
		}
		if (token.source == "if") {
			Exception exception = parseIfStatement(input, index);
			if (exception.type == Nothing) continue;
			return Exception(exception.type, getLineIndex(input, index));
		}
		if (token.source == "while") {
			Exception exception = parseWhileStatement(input, index);
			if (exception.type == Nothing) continue;
			return Exception(exception.type, getLineIndex(input, index));
		}
		auto returnedExpression = parseExpression(input, index);
		if (returnedExpression.exception.type != Nothing) {
			return Exception(returnedExpression.exception.type, getLineIndex(input, index));
		}
		// execute(returnedExpression.source);
	}
	return Nothing;
}

Exception execute(Expression expression) {
	return Exception(Nothing);
}

#endif //NEKO_INTERPRETER_EXECUTION_HPP
