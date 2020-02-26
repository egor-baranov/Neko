#ifndef NEKO_INTERPRETER_OPERATIONPROCESSING_HPP
#define NEKO_INTERPRETER_OPERATIONPROCESSING_HPP

#include "exceptions.h"
#include <cmath>

enum Type {
  IntType,
  FloatType,
  StringType,
  CharType,
  BoolType,
  NoneType,
  OperationType,
  UndefinedType,
};



int processOperation(int b, int a, string op) {
	if (op == "+") {
		return a + b;
	}
	if (op == "-") {
		return a - b;
	}
	if (op == "*") {
		return a * b;
	}
	if (op == "/") {
		return a / b;
	}
	if (op == "**") {
		return pow(a, b);
	}
	if (op == "%") {
		return a % b;
	}
}

double processOperation(double b, double a, string op) {
	if (op == "+") {
		return a + b;
	}
	if (op == "-") {
		return a - b;
	}
	if (op == "*") {
		return a * b;
	}
	if (op == "/") {
		return a / b;
	}
	if (op == "**") {
		return pow(a, b);
	}
}

bool processOperation(bool b, bool a, string op) {
	if (op == "&&") {
		return a && b;
	}
	if (op == "||") {
		return a || b;
	}
	if (op == "==") {
		return a == b;
	}
	if (op == "!=") {
		return a != b;
	}
}

#endif //NEKO_INTERPRETER_OPERATIONPROCESSING_HPP
