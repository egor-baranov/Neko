#ifndef NEKO_INTERPRETER_EXECUTION_HPP
#define NEKO_INTERPRETER_EXECUTION_HPP

#include "token.hpp"
#include "FunctionObject.hpp"
#include "VariableObject.hpp"
#include "ClassObject.hpp"
#include "Constructions.hpp"

Exception parseIfStatement(const vector<Token> &input, int &index);

Exception formatException(Exception e, int lineIndex) {
	if (e.line != -1) return e;
	return Exception(e.type, lineIndex);
}

Exception executeScope(vector<Token> input) {
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
		    (token.type == Name and contain({":=", ":"}, nextToken.source) or
		                            (token.type == Name and nameDeclaration(token.source) == Undeclared and
		                             nextToken.source == "="))) {
			Exception exception = parseVariableDeclaration(input, index);
			if (exception.type == Nothing) {
				continue;
			}
			return formatException(exception, getLineIndex(input, index));
		}
		if (token.source == "fun") {
			auto result = parseFunctionDeclaration(input, index);
			if (result.exception.type == Nothing) {
				continue;
			}
			return formatException(result.exception, getLineIndex(input, index));
		}
		if (token.source == "class") {
			Exception exception = parseClassDeclaration(input, index);
			if (exception.type == Nothing) {
				continue;
			}
			return formatException(exception, getLineIndex(input, index));
		}

		if (nameDeclaration(token.source) != Undeclared and nextToken.type == AssignmentOperator) {
			Exception exception = (nextToken.source == ":=" ? parseVariableDeclaration(input, index) :
			                       parseVariableAssignment(input, index).exception);
			if (exception.type == Nothing) {
				continue;
			}
			return formatException(exception, getLineIndex(input, index));
		}
		// TODO: перенести в parseExpression
//		if (nameDeclaration(token.source) == DeclaredFunction) {
//			Exception exception = parseFunctionCall(input, index).exception;
//			if (exception.type == Nothing) {
//				continue;
//			}
//			return formatException(exception, getLineIndex(input, index));
//		}
		if (token.source == "if") {
			Exception exception = parseIfStatement(input, index);
			if (exception.type == Nothing) {
				continue;
			}
			return formatException(exception, getLineIndex(input, index));
		}
		if (token.source == "while") {
			Exception exception = parseWhileStatement(input, index);
			if (exception.type == Nothing) {
				continue;
			}
			return formatException(exception, getLineIndex(input, index));
		}
		if (token.source == "break") {
			return BREAK;
		}
		if (token.source == "continue") {
			return CONTINUE;
		}
		// TODO: return в возвращаемым значением
		if (token.source == "return") {
			return RETURN;
		}
		auto returnedExpression = parseExpression(input, index);
		if (returnedExpression.exception.type != Nothing) {
			return formatException(returnedExpression.exception, getLineIndex(input, index));
		}
		// execute(returnedExpression.source);
	}
	return Nothing;
}

Exception execute(vector<Token> input) {
	scopeManager.addScope();
	Exception ret = executeScope(input);
	scopeManager.deleteLastScope();
	return ret;
}

Exception execute(vector<Token> input, vector<VariableObject> init) {
	scopeManager.addScope();
	for (VariableObject v: init) {
		Exception exception = scopeManager.add(v);
		if (exception.type != Nothing) {
			return exception;
		}
	}
	Exception ret = executeScope(input);
	scopeManager.deleteLastScope();
	return ret;
}

Exception execute(Expression expression) {
	return Exception(Nothing);
}

#endif //NEKO_INTERPRETER_EXECUTION_HPP
