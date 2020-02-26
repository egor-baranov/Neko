#ifndef NEKO_INTERPRETER_EXPRESSIONS_H
#define NEKO_INTERPRETER_EXPRESSIONS_H

#include "token.h"
#include "exceptions.h"
#include <variant>

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
	return NoneType;
}

void *getValue(const Type &type, const Token token) {
	if (type == StringType) {
		string ret = sliceString(token.source, 1, token.source.size() - 2);
		return static_cast<void *> (new string(ret));
	}
	if (type == CharType) {
		return static_cast<void *>(new char(token.source[1]));
	}
	if (type == IntType) {
		return static_cast<void *>(new int(stoi(token.source)));
	}
	if (type == FloatType) {
		return static_cast<void *>(new double(stod(token.source)));
	}
	if (type == BoolType) {
		return static_cast<void *>(new bool(token.source == "True"));
	}
	if (type == NoneType) {
		return static_cast<void *>(nullptr);
	}
}

struct Item {
  Token token = Token(None, "");
  string source;
  void *value;
  Type type = NoneType;

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
  bool isMutable = false;
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

const FunctionReturned VoidResult = {Item(""), Nothing, true};

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
	stack<Token> variables, operations;
	vector<Item> output;
	int end = input.size();
	for (int i = 0; i < end; ++i) {
		Token token = input[i].token;
		if (token.isEndOfExpression()) {
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

			} else {
				output.push_back(Item(token));
			}
		}
	}
}

// применение оператора к двум операндам
void *Process(Item &a, Item &b, Token op) {

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
	// вычисление выражений
//	for (auto elem: intoPostfixNotation(expression.content)) {
//
//	}
	return Item(emptyToken);
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
	while (index < end) {
		token = input[index];
		Token prevToken = prev(input, index), nextToken = next(input, index);
		if (token.type == Name) {
			if (nameDeclaration(token.source) == Undeclared) {
				return Exception(UndefinedNameUsage);
			}
			if (nameDeclaration(token.source) == DeclaredVariable and nextToken.source != ".") {
				ret.content.push_back(Variables[token.source].item);
				index = nextIndex(input, index);
				break; // потом заменить на continue
			}
			if (nameDeclaration(token.source) == DeclaredFunction) {
				FunctionReturned functionReturned = parseFunctionCall(input, index);
				if (functionReturned.exception.type != Nothing) {
					return functionReturned.exception;
				}
				ret.content.push_back(functionReturned.item);

				break; // потом заменить на continue
			}
		}
		ret.content.push_back(Item(token));
		index = nextIndex(input, index);
		break;
	}
	return ret;
}

#endif //NEKO_INTERPRETER_EXPRESSIONS_H
