#ifndef NEKO_INTERPRETER_OPERATIONPROCESSING_HPP
#define NEKO_INTERPRETER_OPERATIONPROCESSING_HPP

#include "exceptions.hpp"
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

vector<string> possibleOperations(Type argType) {
	switch (argType) {
		case IntType: {
			return {"+", "-", "*", "/", "**", "%", "==", "!=", ">=", "<=", ">", "<"};
		}
		case FloatType: {
			return {"+", "-", "*", "/", "**", "%", "==", "!=", ">=", "<=", ">", "<"};
		}
		case StringType: {
			return {"+", "==", "!=", ">=", "<=", ">", "<"};
		}
		case CharType: {
			return {"==", "!=", ">=", "<=", ">", "<"};
		}
		case BoolType: {
			return {"&&", "||", "==", "!="};
		}
		case NoneType: {
			return {};
		}
		case OperationType: {
			return {};
		}
		case UndefinedType: {
			return {};
		}
	}
}

bool isComparisonOperation(string op) {
	return contain({"==", "!=", ">=", "<=", ">", "<"}, op);
}

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
		return pow(b, a);
	}
	if (op == "%") {
		return a % b;
	}
}

bool compare(int b, int a, string op) {
	if (op == "==") {
		return a == b;
	}
	if (op == "!=") {
		return a != b;
	}
	if (op == ">=") {
		return a >= b;
	}
	if (op == "<=") {
		return a <= b;
	}
	if (op == ">") {
		return a > b;
	}
	if (op == "<") {
		return a < b;
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

bool compare(double b, double a, string op) {
	if (op == "==") {
		return a == b;
	}
	if (op == "!=") {
		return a != b;
	}
	if (op == ">=") {
		return a >= b;
	}
	if (op == "<=") {
		return a <= b;
	}
	if (op == ">") {
		return a > b;
	}
	if (op == "<") {
		return a < b;
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

string processOperation(string b, string a, string op) {
	if (op == "+") {
		return a + b;
	}
}

bool compare(string b, string a, string op) {
	if (op == "==") {
		return a == b;
	}
	if (op == "!=") {
		return a != b;
	}
	if (op == ">=") {
		return a >= b;
	}
	if (op == "<=") {
		return a <= b;
	}
	if (op == ">") {
		return a > b;
	}
	if (op == "<") {
		return a < b;
	}
}

#endif //NEKO_INTERPRETER_OPERATIONPROCESSING_HPP