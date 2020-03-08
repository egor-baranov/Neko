#ifndef NEKO_INTERPRETER_EXPRESSIONS_HPP
#define NEKO_INTERPRETER_EXPRESSIONS_HPP

#include "token.hpp"
#include <variant>
#include "OperationProcessing.hpp"
#include "execution.hpp"

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
		case IntType: {
			return static_cast<void *>(new int(stoi(token.source)));
		}
		case FloatType: {
			return static_cast<void *>(new double(stod(token.source)));
		}
		case StringType: {
			string ret = sliceString(token.source, 1, token.source.size() - 2);
			return static_cast<void *> (new string(ret));
		}
		case CharType: {
			return static_cast<void *>(new char(token.source[1]));
		}
		case BoolType: {
			return static_cast<void *>(new bool(token.source == "True"));
		}
		case NoneType: {
			return static_cast<void *>(nullptr);
		}
		case OperationType: {
			break;
		}
		case UndefinedType: {
			break;
		}
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

  bool isNumber() {
	  return contain({FloatType, IntType}, type);
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
	  token = getToken(source);
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
		  return formatFloatNumber(to_string(*static_cast<double *>(value)));
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

Exception execute(vector<Token> input);

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
  vector<Token> representation;
  vector<FunctionArgument> args;

  Exception runWithArgs(vector<Item> variables) {
	  if (variables.size() < args.size()) {
		  return Exception(FunctionArgumentLack, startIndex);
	  }

	  if (variables.size() > args.size()) {
		  return Exception(FunctionArgumentExcess, startIndex);
	  }

	  execute(representation);
	  return Nothing;
  }
};

set<string> BuiltInFunctions{
	"print", "println",
	"readLine", "readInt", "readFloat", "readChar", "readString", "readBool",
	"sin", "cos", "tg", "ctg"
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

bool isLeftAssociative(Item item) {
	assert(item.type == OperationType);
	return item.source != "**";
}

// TODO: добавить унарные операции
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
			while (not operations.empty()) {
				if (operations.top().source == "(") {
					break;
				}
				if (isLeftAssociative(token) and getPriority(operations.top()) < getPriority(token)) {
					break;
				}
				if (not isLeftAssociative(token) and getPriority(operations.top()) <= getPriority(token)) {
					break;
				}
				output.push_back(operations.top());
				operations.pop();
			}
			operations.push(token);
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

bool isNumber(Item item) {
	return contain({IntType, FloatType}, item.type);
}

bool possibleToProcess(Item &a, Item &b, Token op) {
	if (a.type == b.type) {
		return true;
	}
	if (isNumber(a) and isNumber(b)) {
		return true;
	}
	if (op.source == "*" and
	    ((a.type == IntType and b.type == StringType) or (a.type == StringType and b.type == IntType))) {
		return true;
	}
	if (op.source == "+" and
	    ((a.type == CharType and b.type == StringType) or (a.type == StringType and b.type == CharType))) {
		return true;
	}
}

// применение оператора к двум операндам
ProcessReturned Process(Item &a, Item &b, Token op) {
	if (a.type != b.type and not(isNumber(a) and isNumber(b)) and not possibleToProcess(a, b, op)) {
		return Exception(TypeError);
	}
	// умножение строки на число
	if (op.source == "*") {
		if (a.type == StringType and b.type == IntType) {
			string s = *static_cast<string *>(a.value);
			int x = *static_cast<int *>(b.value);
			return Item(static_cast<void *>(new string(multiply(s, x))), StringType);
		}
		if (a.type == IntType and b.type == StringType) {
			string s = *static_cast<string *>(b.value);
			int x = *static_cast<int *>(a.value);
			return Item(static_cast<void *>(new string(multiply(s, x))), StringType);
		}
	}
	// сложение String и Char
	if (op.source == "+") {
		if (a.type == StringType and b.type == CharType) {
			string s = *static_cast<string *>(a.value);
			char x = *static_cast<char *>(b.value);
			return Item(static_cast<void *>(new string(x + s)), StringType);
		}
		if (a.type == CharType and b.type == StringType) {
			string s = *static_cast<string *>(b.value);
			char x = *static_cast<char *>(a.value);
			return Item(static_cast<void *>(new string(s + x)), StringType);
		}
	}
	if (not contain(possibleOperations(a.type), op.source)) {
		return Exception(OperandTypeError);
	}
	if (a.type == IntType and b.type == IntType) {
		int x = *static_cast<int *>(a.value), y = *static_cast<int *>(b.value);
		if (op.source == "/" and x == 0) {
			return Exception(ZeroDivisionError);
		}
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), BoolType);
		}
		return Item(static_cast<void *>(new int(processOperation(x, y, op.source))), IntType);
	}
	if (a.type == FloatType or b.type == FloatType) {
		double x = (a.type == IntType ? *static_cast<int *>(a.value) : *static_cast<double *>(a.value)),
			y = (b.type == IntType ? *static_cast<int *>(b.value) : *static_cast<double *>(b.value));
		if (op.source == "/" and x == 0) {
			return Exception(ZeroDivisionError);
		}
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), BoolType);
		}
		return Item(static_cast<void *>(new double(processOperation(x, y, op.source))), FloatType);
	}
	if (a.type == BoolType) {
		bool x = *static_cast<bool *>(a.value), y = *static_cast<bool *>(b.value);
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), BoolType);
		}
		return Item(static_cast<void *>(new bool(processOperation(x, y, op.source))), BoolType);
	}
	if (a.type == StringType) {
		string x = *static_cast<string *>(a.value), y = *static_cast<string *>(b.value);
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), BoolType);
		}
		return Item(static_cast<void *>(new string(processOperation(x, y, op.source))), StringType);
	}
	if (a.type == CharType) {
		char x = *static_cast<char *>(a.value), y = *static_cast<char *>(b.value);
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), BoolType);
		}
		return Item(static_cast<void *>(new char(processOperation(x, y, op.source))), CharType);
	}
}

ProcessReturned ProcessUnary(Item &a, Token op) {
	if (not contain({IntType, FloatType, BoolType}, a.type)) {
		return Exception(IncorrectOperationArguments);
	}
	if (not contain({"+", "-", "!"}, op.source)) {
		return Exception(OperationArgumentExcess);
	}
	if (a.type == IntType) {
		int x = *static_cast<int *>(a.value);
		if (op.source == "-") x = -x;
		if (op.source == "!") x = !x;
		return Item(static_cast<void *>(new int(x)), IntType);
	}
	if (a.type == FloatType) {
		double x = *static_cast<double *>(a.value);
		if (op.source == "-") x = -x;
		if (op.source == "!") x = !x;
		return Item(static_cast<void *>(new double(x)), FloatType);
	}
	if (a.type == BoolType) {
		bool x = *static_cast<bool *>(a.value);
		if (op.source != "!") {
			return Exception(IncorrectOperationArguments);
		}
		return Item(static_cast<void *>(new bool(!x)), BoolType);
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
	stack<Item> values;
	// вычисление выражений
	for (auto elem: postfixNotation) {
		if (elem.type == OperationType) {
			if (values.empty()) {
				return Exception(RuntimeError);
			}
			Item a = values.top();
			values.pop();
			if (values.empty() or elem.source == "!") {
				auto processedUnary = ProcessUnary(a, elem.token);
				if (processedUnary.exception.type != Nothing) {
					return processedUnary.exception;
				}
				values.push(processedUnary.item);
				continue;
			}
			Item b = values.top();
			values.pop();
			auto processed = Process(a, b, elem.token);
			if (processed.exception.type != Nothing) {
				return processed.exception;
			}
			values.push(processed.item);
		} else {
			values.push(elem);
		}
	}
	return values.top();
}

struct VariableAssignmentReturned {
  Item item;
  Exception exception;

  VariableAssignmentReturned(Item i, Exception e) : item(i), exception(e) {}

  VariableAssignmentReturned(Item i) : item(i), exception(Nothing) {}

  VariableAssignmentReturned(Exception e) : item(""), exception(e) {}
};

VariableAssignmentReturned parseVariableAssignment(const vector<Token> &input, int &index);

struct ParseExpressionReturned {
  Expression source;
  Exception exception = Exception(Nothing);

  ParseExpressionReturned(Expression s, Exception e) : source(s), exception(e) {}

  ParseExpressionReturned(Expression s) : source(s), exception(Nothing) {}

  ParseExpressionReturned(Exception e) : exception(e) {}
};


// TODO: FIX
ParseExpressionReturned parseExpression(const vector<Token> &input, int &index) {
	Token token = input[index];
	int end = input.size() - 1;
	Expression ret;
	stack<Token> bracketStack;
	int firstIndex = index;
	while (index < end) {
		token = input[index];
		Token prevToken = prev(input, index), nextToken = next(input, index);
		// условия выхода из expression-а
		if (token.type == EOE or (token.isRightBracket() and bracketStack.empty()) or token.source == ",") {
			if (index != firstIndex) {
				// index = nextIndex(input, index);
				break;
			}
		}
		if (index != 0) {
			if ((prevToken.isObject() or prevToken.isRightBracket() or prevToken.isKeyword()) and
			    (token.isObject() or token.isLeftBracket() or token.isKeyword())) {
				if (index != firstIndex) {
					// index = nextIndex(input, index);
					break;
				}
			}
		}
		if (token.isBracket()) {
			if (bracketStack.empty()) {
				bracketStack.push(token);
			} else {
				if (isBracketPair(bracketStack.top(), token)) {
					bracketStack.pop();
				} else {
					bracketStack.push(token);
				}
			}
		}
		if (token.type == Name) {
			if (nameDeclaration(token.source) == Undeclared) {
				return Exception(UndefinedNameUsage, getLineIndex(input, index));
			}
			if (nameDeclaration(token.source) == DeclaredVariable and nextToken.source != ".") {
				if (nextToken.source == "=") {
					auto result = parseVariableAssignment(input, index);
					if (result.exception.type != Nothing) {
						return Exception(result.exception.type, getLineIndex(input, index));
					}
					ret.content.push_back(result.item);
					continue;
				}
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
//	if (not contain({EOE, EOL}, input[index - 1].type) and not contain({EOE, EOL}, token.type) and
//	    not token.isRightBracket() and token.source != ",") {
//		return Exception(SyntaxError);
//	}
	return ret;
}

#endif //NEKO_INTERPRETER_EXPRESSIONS_HPP