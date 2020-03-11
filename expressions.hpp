#ifndef NEKO_INTERPRETER_EXPRESSIONS_HPP
#define NEKO_INTERPRETER_EXPRESSIONS_HPP

#include "token.hpp"
#include <variant>
#include "OperationProcessing.hpp"
#include "execution.hpp"
#include "Object.hpp"

string getType(Token token) {
	if (token.type == IntNumber) {
		return toString(IntType);
	}
	if (token.type == FloatNumber) {
		return toString(FloatType);
	}
	if (token.type == StringLiteral) {
		return toString(StringType);
	}
	if (token.type == CharLiteral) {
		return toString(CharType);
	}
	if (contain({"True", "False"}, token.source)) {
		return toString(BoolType);
	}
	if (token.source == "None") {
		return toString(NoneType);
	}
	if (token.isOperator()) {
		return toString(OperationType);
	}
	return toString(NoneType);
}

void *getValue(const string &type, const Token token) {
	switch (toType(type)) {
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
  public:
  Token token = Token(None, "");
  string source;
  void *value;
  string type = "NoneType";

  ~Item() {
	  if (value != NULL) {
		  switch (toType(type)) {
			  case IntType: {
				  int tmpInt = *static_cast<int *>(value);
				  break;
			  }
			  case FloatType: {
				  float tmpFloat = *static_cast<float *>(value);
				  break;
			  }
			  case StringType: {
				  string tmpString = *static_cast<string *>(value);
				  break;
			  }
			  case CharType: {
				  char tmpChar = *static_cast<char *>(value);
				  break;
			  }
			  case BoolType: {
				  bool tmpBool = *static_cast<bool *>(value);
				  break;
			  }
			  case NoneType: {
				  nullptr_t tmpNullptr = *static_cast<nullptr_t *>(value);
				  break;
			  }
			  case OperationType:
				  break;
			  case UndefinedType:
				  break;
		  }
	  }
  }

  bool isNumber() {
	  return contain({"Float", "Int"}, type);
  }

  bool isNotNumber() {
	  return not isNumber();
  }

  Item(Token init) {
	  token = init;
	  type = getType(init);
	  value = getValue(type, init);
	  source = init.source;
  }

  Item(Token init, string t) {
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

  Item(void *v, string t) : value(v), type(t) {
	  source = toString();
	  if (t == "String") {
		  token = Token(StringLiteral, source);
	  } else if (t == "Char") {
		  token = Token(CharLiteral, source);
	  } else token = getToken(source);
  }

  string toString() {
	  switch (toType(type)) {
		  case StringType: {
			  return *static_cast<string *>(value);
		  }
		  case CharType: {
			  return string(1, *static_cast<char *>(value));
		  }
		  case IntType: {
			  return to_string(*static_cast<int *>(value));
		  }
		  case FloatType: {
			  return formatFloatNumber(to_string(*static_cast<double *>(value)));
		  }
		  case BoolType: {
			  return *static_cast<bool *>(value) ? "True" : "False";
		  }
		  case NoneType: {
			  return "None";
		  }
		  case OperationType:
			  break;
		  case UndefinedType:
			  break;
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
  string type = "Any";
  Item item = Item(emptyToken);
};

struct FunctionArgument {
  bool ref = false;
  string name = "";
  string type = "Any";
};

struct Function : Object {
  bool isLambda = false;
  string name = "";
  string type = "Any";
  int startIndex = -1000;
  vector<Token> representation;
  vector<FunctionArgument> args;

  Exception runWithArgs(vector<Item> variables) {
	  if (variables.size() < args.size()) {
		  return Exception(FunctionArgumentLack, startIndex);
	  }

	  if (variables.size() > args.size()) {
		  return Exception(FunctionArgumentExcess, startIndex);
	  }

	  for (int i = 0; i < args.size(); ++i) {
		  if (args[i].type == variables[i].type or args[i].type == "Any") {
			  continue;
		  }
		  return TypeError;
	  }

	  Exception exception = execute(representation);
	  if (exception.type != Nothing) {
		  return Exception(exception.type, exception.line + startIndex);
	  }
	  return Nothing;
  }
};

set<string> BuiltInFunctions{
	"print", "println",
	"readLine", "readInt", "readFloat", "readChar", "readString", "readBool",
	"abs", "sqr", "sqrt",
	"sin", "cos", "tg", "tan", "ctg", "ctan",
	"asin", "acos", "atan", "atan2",
	"log", "log2", "log10", "ln", "lg",
	"rad2Deg", "deg2Rad", "ceil", "floor",
	"min", "max", "sum", "mul"
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
map<string, Function> Functions;
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
	assert(toType(item.type) == OperationType);
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
	return contain({toString(IntType), toString(FloatType)}, item.type);
}

bool possibleToProcess(Item &a, Item &b, Token op) {
	if (a.type == b.type) {
		return true;
	}
	if (isNumber(a) and isNumber(b)) {
		return true;
	}
	if (op.source == "*" and
	    ((a.type == "Int" and b.type == "String") or (a.type == "String" and b.type == "Int"))) {
		return true;
	}
	if (op.source == "+" and
	    ((a.type == "Char" and b.type == "String") or (a.type == "String" and b.type == "Char"))) {
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
		if (a.type == "String" and b.type == "Int") {
			string s = *static_cast<string *>(a.value);
			int x = *static_cast<int *>(b.value);
			return Item(static_cast<void *>(new string(multiply(s, x))), "String");
		}
		if (a.type == "Int" and b.type == "String") {
			string s = *static_cast<string *>(b.value);
			int x = *static_cast<int *>(a.value);
			return Item(static_cast<void *>(new string(multiply(s, x))), "String");
		}
	}
	// сложение String и Char
	if (op.source == "+") {
		if (a.type == "String" and b.type == "Char") {
			string s = *static_cast<string *>(a.value);
			char x = *static_cast<char *>(b.value);
			return Item(static_cast<void *>(new string(x + s)), "String");
		}
		if (a.type == "Char" and b.type == "String") {
			string s = *static_cast<string *>(b.value);
			char x = *static_cast<char *>(a.value);
			return Item(static_cast<void *>(new string(s + x)), "String");
		}
	}
	if (not contain(possibleOperations(toType(a.type)), op.source)) {
		return Exception(OperandTypeError);
	}
	if (a.type == "Int" and b.type == "Int") {
		int x = *static_cast<int *>(a.value), y = *static_cast<int *>(b.value);
		if (op.source == "/" and x == 0) {
			return Exception(ZeroDivisionError);
		}
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), "Bool");
		}
		return Item(static_cast<void *>(new int(processOperation(x, y, op.source))), "Int");
	}
	if (a.type == "Float" or b.type == "Float") {
		double x = (a.type == "Int" ? *static_cast<int *>(a.value) : *static_cast<double *>(a.value)),
			y = (b.type == "Int" ? *static_cast<int *>(b.value) : *static_cast<double *>(b.value));
		if (op.source == "/" and x == 0) {
			return Exception(ZeroDivisionError);
		}
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), "Bool");
		}
		return Item(static_cast<void *>(new double(processOperation(x, y, op.source))), "Float");
	}
	if (a.type == "Bool") {
		bool x = *static_cast<bool *>(a.value), y = *static_cast<bool *>(b.value);
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), "Bool");
		}
		return Item(static_cast<void *>(new bool(processOperation(x, y, op.source))), "Bool");
	}
	if (a.type == "String") {
		string x = *static_cast<string *>(a.value), y = *static_cast<string *>(b.value);
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), "Bool");
		}
		return Item(static_cast<void *>(new string(processOperation(x, y, op.source))), "String");
	}
	if (a.type == "Char") {
		char x = *static_cast<char *>(a.value), y = *static_cast<char *>(b.value);
		if (isComparisonOperation(op.source)) {
			return Item(static_cast<void *>(new bool(compare(x, y, op.source))), "Bool");
		}
		return Item(static_cast<void *>(new char(processOperation(x, y, op.source))), "Char");
	}
}

ProcessReturned ProcessUnary(Item &a, Token op) {
	if (not contain({"Int", "Float", "Bool"}, a.type)) {
		return Exception(IncorrectOperationArguments);
	}
	if (not contain({"$+", "$-", "!"}, op.source)) {
		return Exception(OperationArgumentExcess);
	}
	if (a.type == "Int") {
		int x = *static_cast<int *>(a.value);
		if (op.source == "$-") x = -x;
		if (op.source == "!") x = !x;
		return Item(static_cast<void *>(new int(x)), "Int");
	}
	if (a.type == "Float") {
		double x = *static_cast<double *>(a.value);
		if (op.source == "$-") x = -x;
		if (op.source == "!") x = !x;
		return Item(static_cast<void *>(new double(x)), "Float");
	}
	if (a.type == "Bool") {
		bool x = *static_cast<bool *>(a.value);
		if (op.source != "!") {
			return Exception(IncorrectOperationArguments);
		}
		return Item(static_cast<void *>(new bool(!x)), "Bool");
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
		if (elem.type == "Operation") {
			if (values.empty()) {
				return Exception(RuntimeError);
			}
			Item a = values.top();
			values.pop();
			if (values.empty() or elem.token.isUnaryOperator()) {
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

// TODO: add method call
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