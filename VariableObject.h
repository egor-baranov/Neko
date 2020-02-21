#ifndef NEKO_INTERPRETER_VARIABLEOBJECT_H
#define NEKO_INTERPRETER_VARIABLEOBJECT_H

#include "token.h"
#include "exceptions.h"
#include "expressions.h"

struct VariableObject {
  bool isMutable = false;
  string name = "";
  string typeName = "Any";
  Item item = Item(emptyToken);
};

struct ClassObject {
  string name = "";
  string parentName = "Object";
};


bool operator>(VariableObject a, VariableObject b) {
	return a.name > b.name;
}

bool operator<(VariableObject a, VariableObject b) {
	return a.name < b.name;
}

bool operator==(VariableObject a, VariableObject b) {
	return a.name == b.name;
}

map<string, VariableObject> Variables;
map<string, ClassObject> Classes;

bool isDeclaredClass(Token token) {
	return Classes.find(token.source) != Classes.end();
}

Exception parseVariableDeclaration(const vector<Token> &input, int &index, Namespace nameSpace = Global) {
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
	if (not contain({"=", ":"}, input[index].source)) {
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
		variable.typeName = input[index].source;
		index = nextIndex(input, index);
	}
	if (input[index].source != "=") {
		return Exception(VariableDeclarationError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	ParseExpressionReturned result = parseExpression(input, index);
	if (result.exception.type != Nothing) {
		return result.exception;
	}
	variable.item = Calculate(result.source);
	Variables[variable.name] = variable;
	return Exception(Nothing);
}


#endif //NEKO_INTERPRETER_VARIABLEOBJECT_H
