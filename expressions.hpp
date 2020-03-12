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
		case FunctionType:
			break;
	}
}

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
};


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
			  case FunctionType: {
				  // Function tmpFunction = *static_cast<Function *>(value);
				  break;
			  }
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
			  return "OperationObject";
		  case UndefinedType:
			  return "UndefinedObject";
		  case FunctionType:
			  return "FunctionObject";
	  }
	  assert(false);
  }
};

// последовательности токенов (или item-ов, я пока хз), преобразованные в постфиксную запись для быстрого вычисления
struct Expression {
  vector<Item> content;
};

struct VariableObject {
  bool isMutable = true;
  string name = "";
  string type = "Any";
  Item item = Item(emptyToken);

  VariableObject() {

  }
};


Exception execute(vector<Token> input);

Exception execute(vector<Token> input, vector<VariableObject> init);

Exception runWithArgs(Function function, vector<Item> init) {
	if (init.size() < function.args.size()) {
		return Exception(FunctionArgumentLack, function.startIndex);
	}

	if (init.size() > function.args.size()) {
		return Exception(FunctionArgumentExcess, function.startIndex);
	}

	for (int i = 0; i < function.args.size(); ++i) {
		if (function.args[i].type == init[i].type or function.args[i].type == "Any") {
			continue;
		}
		return TypeError;
	}

	vector<VariableObject> variables;

	for (int i = 0; i < function.args.size(); ++i) {
		VariableObject v;
		v.item = init[i];
		v.type = function.args[i].type;
		v.name = function.args[i].name;
		variables.push_back(v);
	}

	// TODO: add args
	Exception exception = execute(function.representation, variables);
	if (exception.type != Nothing) {
		return Exception(exception.type, exception.line + function.startIndex);
	}
	return Nothing;
}

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

//map<string, VariableObject> Variables;
//map<string, Function> Functions;
map<string, ClassObject> Classes;

struct ScopeManager {
  public:
  vector<map<string, VariableObject>> m;

  void addScope() {
	  m.push_back(map<string, VariableObject>());
  }

  Exception add(VariableObject v) {
	  if (m.back().find(v.name) != m.back().end()) {
		  return RedefinationError;
	  }
	  m.back()[v.name] = v;
	  return Nothing;
  }

  VariableObject get(string name) {
	  for (int i = m.size() - 1; i >= 0; --i) {
		  if (m[i].find(name) != m[i].end()) {
			  return m[i][name];
		  }
	  }
	  assert(false);
  }

  void setItem(string name, Item item) {
	  for (int i = m.size() - 1; i >= 0; --i) {
		  if (m[i].find(name) != m[i].end()) {
			  m[i][name].item = item;
			  return;
		  }
	  }
	  assert(false);
  }

  Function getFunction(string name) {
	  VariableObject obj = get(name);

	  return *static_cast<Function *>(obj.item.value);
  }

  bool find(string name) {
	  for (int i = m.size() - 1; i >= 0; --i) {
		  if (m[i].find(name) != m[i].end()) {
			  return true;
		  }
	  }
	  return false;
  }

  void deleteLastScope() {
	  m.pop_back();
  }
};

ScopeManager scopeManager;

enum nameType {
  DeclaredVariable,
  DeclaredFunction,
  DeclaredClass,
  Undeclared
};

nameType nameDeclaration(string name) {
	if (BuiltInFunctions.find(name) != BuiltInFunctions.end()) {
		return DeclaredFunction;
	}
	if (scopeManager.find(name)) {
		VariableObject obj = scopeManager.get(name);
		if (obj.type == "Function" or obj.item.type == "Function") {
			return DeclaredFunction;
		}
		return DeclaredVariable;
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
				output.push_back(input[i]);
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
ProcessReturned Process(Item a, Item b, Token op) {
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
//	if (expression.content.size() == 1) {
//		Item ret = expression.content[0];
//		if (ret.token.type == Name) {
//			if (scopeManager.find(ret.token.source)) {
//				return Exception(UndefinedNameUsage);
//			}
//			return scopeManager.get(ret.token.source).item;
//		}
//		return ret;
//	}

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
	if (values.size() != 1) {
		return Exception(RuntimeError);
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

struct FunctionDeclarationParsed {
  Function function;
  Exception exception;

  FunctionDeclarationParsed(Function f, Exception e) : function(f), exception(e) {}

  FunctionDeclarationParsed(Function f) : function(f), exception(Nothing) {}

  FunctionDeclarationParsed(Exception e) : function(Function()), exception(e) {}
};

FunctionDeclarationParsed parseFunctionDeclaration(const vector<Token> &input, int &index);

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
			    (token.isObject() or token.isLeftBracket() or token.isKeyword() or token.isUnaryOperator())) {
				if (index != firstIndex) {
					if (contain({EOL, EOE}, input[index - 1].type)) {
						// index = nextIndex(input, index);
						break;
					} else {
						return Exception(SyntaxError, getLineIndex(input, index));
					}
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

		// parse call TODO: change to isCallable()
		if (token.isObject() and input[index + 1].source == "(") {
			if (token.type == Name and nameDeclaration(token.source) == DeclaredFunction) {
				FunctionReturned functionReturned = parseFunctionCall(input, index);
				if (functionReturned.exception.type != Nothing) {
					return functionReturned.exception;
				}
				ret.content.push_back(functionReturned.item);
			} else {
				return Exception(CallError, getLineIndex(input, index));
			}
			continue;
		}

		// parse indexation
		if (token.isObject() and nextToken.source == "[") {
			continue;
		}

		// parse ternary operator
		if (token.source == "if") {
			continue;
		}

		// parse lambda
		if (token.source == "lambda") {
			auto result = parseFunctionDeclaration(input, index);
			if (result.exception.type != Nothing) {
				return result.exception;
			}
			ret.content.push_back(Item(static_cast<void *>(new Function(result.function)), "Function"));
			continue;
		}

		// обращение к полю или методу
		if (token.isObject() and input[index + 1].source == ".") {
			continue;
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
				ret.content.push_back(scopeManager.get(token.source).item);
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
		// cout << "[" << token.source << "]" << endl;
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