#ifndef NEKO_INTERPRETER_FUNCTIONOBJECT_H
#define NEKO_INTERPRETER_FUNCTIONOBJECT_H

#include "VariableObject.h"
#include "expressions.h"

struct FunctionArgument {
  bool ref = false;
  string name = "";
  string typeName = "Any";
};

struct FunctionObject {
  bool isLambda = false;
  string name = "";
  string typeName = "Any";
  int startIndex = -1;
  vector<Expression> representation;
  vector<FunctionArgument> args;

  Exception runWithArgs(vector<VariableObject> variables) {
	  if (variables.size() < args.size()) {
		  return Exception(FunctionArgumentLack, startIndex);
	  }
	  if (variables.size() > args.size()) {
		  return Exception(FunctionArgumentExcess, startIndex);
	  }

  }
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

map<string, FunctionObject> Functions;

Exception parseFunctionDeclaration(const vector<Token> &input, int &index) {
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


#endif //NEKO_INTERPRETER_FUNCTIONOBJECT_H
