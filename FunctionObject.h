#ifndef NEKO_INTERPRETER_FUNCTIONOBJECT_H
#define NEKO_INTERPRETER_FUNCTIONOBJECT_H

#include "token.h"
#include "exceptions.h"
#include "Variable.h"
#include "expressions.h"

struct FunctionArgument {
	bool ref = false;
	string name = "";
	string typeName = "Any";
};

struct FunctionObject {
	bool lambda = false;
	string name = "";
	string typeName = "Any";

	vector<Expression> representation;
	vector<FunctionArgument> args;
};

bool operator>(FunctionObject a, FunctionObject b) {
	return a.name > b.name;
}

bool operator<(FunctionObject a, FunctionObject b) {
	return a.name < b.name;
}

bool operator==(FunctionObject a, FunctionObject b) {
	return a.name == b.name;
}

set<FunctionObject> FunctionSet;

Exception parseFunctionDeclaration(const vector<Token> &input, int &index) {
	if (input.size() < 6) {
		return Exception(FunctionDeclarationError, getLineIndex(input, index));
	}
	FunctionObject functionObject = FunctionObject();
	if (not contain({"fun", "lambda"}, input[index].source)) {
		return Exception(FunctionDeclarationError, getLineIndex(input, index));
	}
	// есть ли имя у функции
	functionObject.lambda = input[index].source == "lambda";
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

	} else {
		index = nextIndex(input, index);

	}
	FunctionSet.insert(functionObject);
	return Exception(Nothing);
}


#endif //NEKO_INTERPRETER_FUNCTIONOBJECT_H
