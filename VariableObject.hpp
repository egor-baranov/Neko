#ifndef NEKO_INTERPRETER_VARIABLEOBJECT_HPP
#define NEKO_INTERPRETER_VARIABLEOBJECT_HPP

#include "token.hpp"
#include "exceptions.hpp"
#include "expressions.hpp"

bool operator>(VariableObject a, VariableObject b) {
	return a.name > b.name;
}

bool operator<(VariableObject a, VariableObject b) {
	return a.name < b.name;
}

bool operator==(VariableObject a, VariableObject b) {
	return a.name == b.name;
}

bool isDeclaredClass(Token token) {
	return Classes.find(token.source) != Classes.end();
}

bool isAssignmentOperator(Item item) {
	return item.token.type == AssignmentOperator;
}

VariableAssignmentReturned parseVariableAssignment(const vector<Token> &input, int &index) {
	if (input.size() < 3) {
		return Exception(VariableDeclarationError, getLineIndex(input, index));
	}
	if (input[index].type != Name) {
		return Exception(VariableAssignmentError, getLineIndex(input, index));
	}
	string name = input[index].source;
	if (nameDeclaration(name) == Undeclared) {
		return Exception(VariableAssignmentError, getLineIndex(input, index));
	}
	if (not scopeManager.get(name).isMutable) {
		return Exception(ConstAssignment, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	if (input[index].type != AssignmentOperator) {
		return Exception(VariableAssignmentError, getLineIndex(input, index));
	}
	Token assignmentOperator = input[index];
	index = nextIndex(input, index);
	ParseExpressionReturned result = parseExpression(input, index);
	if (result.exception.type != Nothing) {
		return Exception(result.exception.type, getLineIndex(input, index));
	}
	CalculateReturned calculateReturned = Calculate(result.source);
	if (calculateReturned.exception.type != Nothing) {
		return Exception(calculateReturned.exception.type, getLineIndex(input, index));
	}
	if (assignmentOperator.source == "=") {
		if (not contain({"Any", calculateReturned.item.type}, scopeManager.get(name).type)) {
			return Exception(TypeError, getLineIndex(input, index));
		}
		scopeManager.setItem(name, calculateReturned.item);
	} else {
		string s = assignmentOperator.source;
		auto processed = Process(calculateReturned.item, scopeManager.get(name).item,
		                         getToken(sliceString(s, 0, s.size() - 2)));
		if (processed.exception.type != Nothing) {
			return Exception(processed.exception.type, getLineIndex(input, index));
		}
		if (not contain({"Any", processed.item.type}, scopeManager.get(name).type)) {
			return Exception(TypeError, getLineIndex(input, index));
		}
		scopeManager.setItem(name, processed.item);
	}
	return scopeManager.get(name).item;
}

Exception parseVariableDeclaration(const vector<Token> &input, int &index) {
	if (input.size() < 3) {
		return Exception(VariableDeclarationError, getLineIndex(input, index));
	}
	VariableObject variable;
	if (contain({"var", "val"}, input[index].source)) {
		variable.isMutable = input[index].source == "var";
		index = nextIndex(input, index);
	}
	if (input[index].type != Name) {
		return Exception(VariableDeclarationError, getLineIndex(input, index));
	}
	variable.name = input[index].source;
	index = nextIndex(input, index);
	if (not contain({"=", ":=", ":"}, input[index].source)) {
		return Exception(VariableDeclarationError, getLineIndex(input, index));
	}
	if (input[index].source == ":") {
		index = nextIndex(input, index);
		if (not contain({Name}, input[index].type)) {
			return Exception(VariableDeclarationError, getLineIndex(input, index));
		}
		if (not isBuildInType(input[index]) and not isDeclaredClass(input[index])) {
			return Exception(UnknownTypeError, getLineIndex(input, index));
		}
		variable.type = input[index].source;
		index = nextIndex(input, index);
	}
	if (not contain({"=", ":="}, input[index].source)) {
		return Exception(VariableDeclarationError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	ParseExpressionReturned result = parseExpression(input, index);
	if (result.exception.type != Nothing) {
		return Exception(result.exception.type, getLineIndex(input, index));
	}
	CalculateReturned calculateReturned = Calculate(result.source);
	if (calculateReturned.exception.type != Nothing) {
		return Exception(calculateReturned.exception.type, getLineIndex(input, index));
	}
	if (variable.type != "Any" and variable.type != calculateReturned.item.type) {
		return Exception(TypeError, getLineIndex(input, index));
	}
	variable.item = calculateReturned.item;
	Exception exception = scopeManager.add(variable);
	if (exception.type != Nothing) {
		return exception;
	}
	return Exception(Nothing);
}


#endif //NEKO_INTERPRETER_VARIABLEOBJECT_HPP
