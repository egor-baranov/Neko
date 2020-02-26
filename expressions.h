#ifndef NEKO_INTERPRETER_EXPRESSIONS_H
#define NEKO_INTERPRETER_EXPRESSIONS_H

#include "token.h"
#include <variant>
#include "OperationProcessing.hpp"

Type getType(Token token) {
	if (token.type == IntNumber) {
		return IntType;
	}
	if (token.type == FloatNumber) {
		return FloatType;
	}
	if (token.type == StringLiteral) {
		return StringType;
	}
	if (token.type == CharLiteral) {
		return CharType;
	}
	if (contain({"True", "False"}, token.source)) {
		return BoolType;
	}
	if (token.source == "None") {
		return NoneType;
	}
	if (token.isOperator()) {
		return OperationType;
	}
	return NoneType;
}

void *getValue(const Type &type, const Token token) {
	switch (type) {
		case IntType:
			return static_cast<void *>(new int(stoi(token.source)));
		case FloatType:
			return static_cast<void *>(new double(stod(token.source)));
		case StringType: {
			string ret = sliceString(token.source, 1, token.source.size() - 2);
			return static_cast<void *> (new string(ret));
		}
		case CharType:
			return static_cast<void *>(new char(token.source[1]));
		case BoolType:
			return static_cast<void *>(new bool(token.source == "True"));
		case NoneType:
			return static_cast<void *>(nullptr);
		case OperationType:
			break;
		case UndefinedType:
			break;
	}
}

struct Item {
  Token token = Token(None, "");
  string source;
  void *value;
  Type type = NoneType;

  ~Item() {
	  // TODO: переопределить деструктор для value
  }

  Item(Token init) {
	  token = init;
	  type = getType(init);
	  value = getValue(type, init);
	  source = init.source;
  }

  Item(Token init, Type t) {
	  token = init;
	  type = t;
	  value = getValue(t, init);
	  source = init.source;
  }

  Item(string s) {
	  Token init = getToken(s);
	  token = init;
	  type = getType(init);
	  value = getValue(type, init);
	  source = init.source;
  }

  Item(void *v, Type t) : value(v), type(t) {
	  source = toString();
  }

  string toString() {
	  if (type == StringType) {
		  return *static_cast<string *>(value);
	  }
	  if (type == CharType) {
		  return string(1, *static_cast<char *>(value));
	  }
	  if (type == IntType) {
		  return to_string(*static_cast<int *>(value));
	  }
	  if (type == FloatType) {
		  return to_string(*static_cast<double *>(value));
	  }
	  if (type == BoolType) {
		  return *static_cast<bool *>(value) ? "True" : "False";
	  }
	  if (type == NoneType) {
		  return "None";
	  }
  }
};

// последовательности токенов (или item-ов, я пока хз), преобразованные в постфиксную запись для быстрого вычисления
struct Expression {
  vector<Item> content;
};

struct VariableObject {
  bool isMutable = true;
  string name = "";
  string typeName = "Any";
  Item item = Item(emptyToken);
};

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

  Exception runWithArgs(vector<Item> variables) {
	  if (variables.size() < args.size()) {
		  return Exception(FunctionArgumentLack, startIndex);
	  }

	  if (variables.size() > args.size()) {
		  return Exception(FunctionArgumentExcess, startIndex);
	  }
  }
};

set<string> BuiltInFunctions{
	"print", "println",
	"readLine", "readInt", "readFloat", "readChar", "readBool"
};

struct ClassObject {
  string name = "";
  string parentName = "Object";
};

struct FunctionReturned {
  Item item;
  Exception exception;
  bool isVoid;

  FunctionReturned(Item i, Exception e, bool v = false) : item(i), exception(e), isVoid(v) {}

  FunctionReturned(Item i) : item(i), exception(Nothing), isVoid(false) {}

  FunctionReturned(Exception e) : item(""), exception(e), isVoid(false) {}
};

FunctionReturned parseFunctionCall(const vector<Token> &input, int &index);

const FunctionReturned VoidResult = {Item(getToken("")), Nothing, true};

map<string, VariableObject> Variables;
map<string, FunctionObject> Functions;
map<string, ClassObject> Classes;

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
	if (Functions.find(name) != Functions.end() or BuiltInFunctions.find(name) != BuiltInFunctions.end()) {
		return DeclaredFunction;
	}
	if (Classes.find(name) != Classes.end()) {
		return DeclaredClass;
	}
	return Undeclared;
}

vector<Item> intoPostfixNotation(vector<Item> input) {
	stack<Token> operations;
	vector<Item> output;
	int end = input.size();
	for (int i = 0; i < end; ++i) {
		Token token = input[i].token;
		if (token.source == "(") {
			operations.push(token);
			continue;
		}
		if (token.isOperator()) {
			stack<Token> opTmp;
			while (not operations.empty()) {
				if (operations.top().source == "(" or getPriority(operations.top()) < getPriority(token)) {
					break;
				}
				opTmp.push(operations.top());
				operations.pop();
			}
			operations.push(token);
			while (not opTmp.empty()) {
				operations.push(opTmp.top());
				opTmp.pop();
			}
		} else {
			if (token.source == ")") {
				while (operations.top().source != "(") {
					output.push_back(operations.top());
					operations.pop();
				}
				operations.pop();
			} else {
				output.push_back(token);
			}
		}
	}
	while (not operations.empty()) {
		output.push_back(operations.top());
		operations.pop();
	}
	return output;
}

struct ProcessReturned {
  Item item;
  Exception exception;

  ProcessReturned(Item i, Exception e = Nothing) : item(i), exception(e) {}

  ProcessReturned(Exception e) : item(""), exception(e) {}
};

// применение оператора к двум операндам
ProcessReturned Process(const Item &a, const Item &b, Token op) {
	if (a.type != b.type) {
		return Exception(TypeError);
	}
	if (a.type == IntType) {
		int x = *static_cast<int *>(a.value), y = *static_cast<int *>(b.value);
		return Item(static_cast<void *>(new int(processOperation(x, y, op.source))), IntType);
	}
	if (a.type == FloatType) {
		double x = *static_cast<double *>(a.value), y = *static_cast<double *>(b.value);
		return Item(static_cast<void *>(new double(processOperation(x, y, op.source))), FloatType);
	}
	if (a.type == BoolType) {
		bool x = *static_cast<bool *>(a.value), y = *static_cast<bool *>(b.value);
		return Item(static_cast<void *>(new bool(processOperation(x, y, op.source))), BoolType);
	}
}

struct CalculateReturned {
  Item item;
  Exception exception = Exception(Nothing);

  CalculateReturned(Item item, Exception e = Nothing) : item(item), exception(e) {}

  CalculateReturned(Exception e) : item(""), exception(e) {}
};

CalculateReturned Calculate(Expression expression) {
	if (expression.content.size() == 1) {
		Item ret = expression.content[0];
		if (ret.token.type == Name) {
			if (Variables.find(ret.token.source) == Variables.end()) {
				return Exception(UndefinedNameUsage);
			}
			return Variables[ret.token.source].item;
		}
		return ret;
	}

	vector<Item> postfixNotation = intoPostfixNotation(expression.content);
	stack<Item> variables;
	// вычисление выражений
	for (auto elem: postfixNotation) {
		if (elem.type == OperationType) {
			Item a = variables.top();
			variables.pop();
			Item b = variables.top();
			variables.pop();
			auto processed = Process(a, b, elem.token);
			if (processed.exception.type != Nothing) {
				return processed.exception;
			}
			variables.push(processed.item);
		} else {
			variables.push(elem);
		}
	}
	return variables.top();
}

struct ParseExpressionReturned {
  Expression source;
  Exception exception = Exception(Nothing);

  ParseExpressionReturned(Expression s, Exception e) : source(s), exception(e) {}

  ParseExpressionReturned(Expression s) : source(s), exception(Nothing) {}

  ParseExpressionReturned(Exception e) : exception(e) {}
};

ParseExpressionReturned parseExpression(const vector<Token> &input, int &index) {
	Token token = input[index];
	int end = input.size() - 1;
	Expression ret;
	stack<Token> bracketStack;
	while (index < end) {
		token = input[index];
		Token prevToken = prev(input, index), nextToken = next(input, index);
		// условия выхода из expression-а
		// TODO: FIX
		if (token.type == EOE or (token.isRightBracket() and bracketStack.empty()) or token.source == ",") {
			break;
		}
		if (index != 0 and prevToken.isObject() and token.isObject()) {
			break;
		}
		if (token.isBracket()) {
			if (bracketStack.empty()) {
				bracketStack.push(token);
			} else {
				if (isBracketPair(bracketStack.top(), token)) {
					bracketStack.pop();
				} else {
					bracketStack.pop();
				}
			}
		}
		if (token.type == Name) {
			if (nameDeclaration(token.source) == Undeclared) {
				return Exception(UndefinedNameUsage);
			}
			if (nameDeclaration(token.source) == DeclaredVariable and nextToken.source != ".") {
				ret.content.push_back(Variables[token.source].item);
				index = nextIndex(input, index);
				continue;
			}
			if (nameDeclaration(token.source) == DeclaredFunction) {
				FunctionReturned functionReturned = parseFunctionCall(input, index);
				if (functionReturned.exception.type != Nothing) {
					return functionReturned.exception;
				}
				ret.content.push_back(functionReturned.item);
				continue;
			}
		}
		ret.content.push_back(Item(token));
		index = nextIndex(input, index);
	}
	return ret;
}

#endif //NEKO_INTERPRETER_EXPRESSIONS_H