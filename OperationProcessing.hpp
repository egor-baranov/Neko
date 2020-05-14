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
  FunctionType
};

string toString(Type t) {
	switch (t) {
		case IntType:
			return "Int";
		case FloatType:
			return "Float";
		case StringType:
			return "String";
		case CharType:
			return "Char";
		case BoolType:
			return "Bool";
		case NoneType:
			return "NoneType";
		case OperationType:
			return "Operation";
		case UndefinedType:
			break;
		case FunctionType:
			return "Function";
	}
}

Type toType(string s) {
	if (s == "Int") {
		return IntType;
	}
	if (s == "Float") {
		return FloatType;
	}
	if (s == "String") {
		return StringType;
	}
	if (s == "Char") {
		return CharType;
	}
	if (s == "Bool") {
		return BoolType;
	}
	if (s == "NoneType") {
		return NoneType;
	}
	if (s == "Operation") {
		return OperationType;
	}
	if (s == "Function") {
		return FunctionType;
	}
}

vector<string> possibleOperations(Type argType) {
	switch (argType) {
		case IntType: {
			return {"+", "-", "*", "/", "**", "%", "&", "|", "^", ">>", "<<",
			        "==", "!=", ">=", "<=", ">", "<"};
		}
		case FloatType: {
			return {"+", "-", "*", "/", "**", "%", "==", "!=", ">=", "<=", ">", "<"};
		}
		case StringType: {
			return {"+", "==", "!=", ">=", "<=", ">", "<"};
		}
		case CharType: {
			return {"+", "-", "*", "/", "**", "%", "&", "|", "^", ">>", "<<",
			        "==", "!=", ">=", "<=", ">", "<"};
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
		case FunctionType:
			break;
	}
}

bool isComparisonOperation(string op) {
	return contain({"==", "!=", ">=", "<=", ">", "<"}, op);
}

long long processOperation(long long b, long long a, string op) {
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
	if (op == "&") {
		return b & a;
	}
	if (op == "|") {
		return b | a;
	}
	if (op == "^") {
		return b ^ a;
	}
	if (op == ">>") {
		return b >> a;
	}
	if (op == "<<") {
		return b << a;
	}
}

long double processOperation(long double b, long double a, string op) {
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

string processOperation(string b, string a, string op) {
	if (op == "+") {
		return a + b;
	}
}

template<typename T>
bool compare(T b, T a, string op) {
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

char processOperation(char b, char a, string op) {
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
	if (op == "&") {
		return b & a;
	}
	if (op == "|") {
		return b | a;
	}
	if (op == "^") {
		return b ^ a;
	}
	if (op == ">>") {
		return b >> a;
	}
	if (op == "<<") {
		return b << a;
	}
}

#endif //NEKO_INTERPRETER_OPERATIONPROCESSING_HPP