#ifndef NEKO_INTERPRETER_OBJECT_HPP
#define NEKO_INTERPRETER_OBJECT_HPP

#include "helpers.hpp"
#include "exceptions.hpp"
#include <set>
#include <map>

using namespace std;

class Object {
  public:

  // сериализация объекта в JSON, в будущем возвращает JSONObject
  virtual void serialize() {};

  // вызов объекта, переопределяется у функций
  virtual void call() {};

  // обращение к объекту посредством оператора []
  virtual void index() {};

  virtual void contain(Object *other) {};

  virtual void slice() {};

  virtual void iterate() {};

  // битовое представление объекта в памяти
  virtual void bit() {};
  void *value;
};

class Int : Object {
  public:
  long long value;

  Int(long long init) : value(init) {}
};

class Float : Object {
  public:
  long double value;

  Float(Int init) : value(init.value) {}

  Float(long double init) : value(init) {}
};

class Char : Object {
  public:
  char value;

  Char(char init) : value(init) {}
};

class Bool : Object {
  public:
  bool value;

  Bool(bool init) : value(init) {}
};

class String : Object {
  public:
  string value;

  String(string init) : value(init) {}

  Int toInt() {
	  string ret;
	  for (int i = 1; i < value.size() - 1; ++i) {
		  ret += value[i];
	  }
	  return Int(stoll(ret));
  }

  Float toFloat() {
	  string ret;
	  for (int i = 1; i < value.size() - 1; ++i) {
		  ret += value[i];
	  }
	  return Float(stold(ret));
  }

  Int length() {
	  return Int(value.size());
  }
};

class NoneT : Object {
  public:
  nullptr_t value;

  NoneT(nullptr_t init) : value(init) {}
};

struct FunctionArgument {
  string type = "Any";

  public:
  bool ref = false;
  string name = "";
};

class Function : public Object {
  private:
  set<string> type{"Any"};

  public:
  bool isLambda = false;
  string name = "";
  int startIndex = -1;
  vector<Token> representation;
  vector<FunctionArgument> args;

  bool isAnyType() {
	  return type.find("Any") != type.end();
  }

  void addType(string t) {
	  if (type.find("Any") != type.end()) {
		  type.clear();
	  }
	  type.insert(t);
  }

  set<string> getType() {
	  return type;
  }

  bool containType(string t) {
	  return type.find(t) != type.end();
  }
};

class Range : Object {
  public:
  Float left, right, step;

  Range(Float r) : left(0), right(r.value - 1), step(1) {}

  Range(Float l, Float r) : left(l), right(r.value - 1), step(1) {}

  Range(Float l, Float r, Float s) : left(l), right(r.value - 1), step(s) {}
};

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
			return static_cast<void *>(new Int(stoll(token.source)));
		}
		case FloatType: {
			return static_cast<void *>(new Float(stold(token.source)));
		}
		case StringType: {
			string ret = sliceString(token.source, 1, token.source.size() - 2);
			return static_cast<void *> (new String(ret));
		}
		case CharType: {
			return static_cast<void *>(new Char(token.source[1]));
		}
		case BoolType: {
			return static_cast<void *>(new Bool(token.source == "True"));
		}
		case NoneType: {
			return static_cast<void *>(new NoneT(nullptr));
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

class Item {
  public:
  Token token = Token(None, "");
  string source;
  void *value;
  string type = "NoneType";

  ~Item() {
	  if (value != NULL) {
		  switch (toType(type)) {
			  case IntType: {
				  Int tmpInt = *static_cast<Int *>(value);
				  break;
			  }
			  case FloatType: {
				  Float tmpFloat = *static_cast<Float *>(value);
				  break;
			  }
			  case StringType: {
				  String tmpString = *static_cast<String *>(value);
				  break;
			  }
			  case CharType: {
				  Char tmpChar = *static_cast<Char *>(value);
				  break;
			  }
			  case BoolType: {
				  Bool tmpBool = *static_cast<Bool *>(value);
				  break;
			  }
			  case NoneType: {
				  NoneT tmpNullptr = *static_cast<NoneT *>(value);
				  break;
			  }
			  case OperationType:
				  break;
			  case UndefinedType:
				  break;
			  case FunctionType: {
				  Function tmpFunction = *static_cast<Function *>(value);
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
			  return static_cast<String *>(value)->value;
		  }
		  case CharType: {
			  return string(1, static_cast<Char *>(value)->value);
		  }
		  case IntType: {
			  return to_string(static_cast<Int *>(value)->value);
		  }
		  case FloatType: {
			  return formatFloatNumber(to_string(static_cast<Float *>(value)->value));
		  }
		  case BoolType: {
			  return static_cast<Bool *>(value)->value ? "True" : "False";
		  }
		  case NoneType: {
			  return "None";
		  }
		  case OperationType:
			  return "OperationObject";
		  case UndefinedType:
			  return "UndefinedObject";
		  case FunctionType:
			  return "Function";
	  }
	  // TODO: fix
	  return "None";
	  assert(false);
  }
};

bool operator>(const Item &item1, const Item &item2) {
	return item1.source > item2.source;
}

bool operator<(const Item &item1, const Item &item2) {
	return item1.source < item2.source;
}

class Pair : Object {
  public:
  Item first, second;

//  Pair(Item item1, Item item2) {
//	  first = Item(item1.value, item1.source);
//	  second = Item(item2.value, item2.source);
//  }
};

struct ContainerReturned {
  Item item;
  Exception exception;

  ContainerReturned(Item i, Exception e) : item(i), exception(e) {}

  ContainerReturned(Item i) : item(i), exception(Nothing) {}

  ContainerReturned(Exception e) : item(""), exception(e) {}
};

class Container : Object {
  protected:
  int length;
  public:
  vector<Item> content;

  virtual Int getLength() {
	  return Int(length);
  }

  virtual Exception add(Item item) {};

  virtual bool isEmpty() {
	  return length == 0;
  }

  virtual void clear() {
	  this->content.clear();
	  length = 0;
  }
};

class Array : Container {
  private:
  int length;
  public:
  vector<Item> content;

  Int getLength() {
	  return Int(length);
  }

  virtual ContainerReturned get(int index) {
	  if (index < 0 or index >= this->length) {
		  return Exception(IndexError);
	  }
  }

  Exception add(Item item) {
	  this->content.push_back(item);
	  this->length += 1;
  }

  Array() : length(0) {}

  Array(int n, Item init = Item("")) : length(0) {}

  Array(Container init) {
	  for (Item item : init.content) {
		  this->add(item);
	  }
  }

  Array(vector<Item> init) {
	  this->content = init;
	  this->length = init.size();
  }
};

class MutableArray : public Array {
  private:
  int length;

  public:
  MutableArray() : length(0) {}

  MutableArray(vector<Item> init) {
	  this->content = init;
	  this->length = init.size();
  }
};

class Stack : Container {
  private:
  int length;
  public:
  stack<Item> content;

  Stack() : length(0) {}

  Int getLength() {
	  return length;
  }

  Exception push(Item item) {
	  content.push(item);
	  ++length;
  }

  ContainerReturned pop() {
	  if (length == 0) {
		  return Exception(EmptyContainerError);
	  }
	  Item ret = content.top();

  }

  ContainerReturned top() {
	  if (length == 0) {
		  return Exception(EmptyContainerError);
	  }
	  return content.top();
  }

  void clear() {
	  while (not content.empty()) {
		  content.pop();
	  }
	  this->length = 0;
  }
};

class Queue : Container {
  private:
  int length;
  public:
  queue<Item> content;
};

class Deque : Container {
  private:
  int length;
  public:
  deque<Item> content;
};

class ArrayList : Array {
  private:
  int length;
  public:
  vector<Item> content;

  Exception add(Item item) {
	  this->content.push_back(item);
  }
};

class MutableArrayList : public ArrayList {
  private:
  int length;

  public:
  MutableArrayList() : length(0) {}
};

class List : public Container {
  private:
  int length;
  public:
  list<Item> content;
};

class MutableList : public List {
  private:
  int length;
  public:
  list<Item> content;
};

class Set : Container {
  private:
  int length;
  public:
  set<Item> content;

  Exception add(Item item) {
	  this->content.insert(item);
	  ++length;
  }

  Exception remove(Item item) {
	  if (content.find(item) == content.end()) {
		  return Exception(RuntimeError);
	  }
  }

  Set() : length(0) {}

  Set(Container init) : length(0) {}
};

class MutableSet : public Set {
  private:
  int length;
  public:
  set<Item> content;

};

class Map : Container {
  private:
  int length;
  public:
  map<void *, Item> content;

};

class MutableMap : Map {
  private:
  int length;
  public:
  map<void *, Item> content;

  MutableMap() : length(0) {}
};

struct ClassObject {
  string name = "";
  string parentName = "Object";
};

map<string, ClassObject> Classes;

struct ParserReturned {
  Item item;
  Exception exception;

  ParserReturned(Item i, Exception e) : item(i), exception(e) {}

  ParserReturned(Item i) : item(i), exception(Nothing) {}

  ParserReturned(Exception e) : item(""), exception(e) {}
};

ParserReturned parseInitializerList(vector<Item> &input, int &index) {

}

ParserReturned parseMethodCall() {

}

ParserReturned callMethod() {

}

// последовательности токенов (или item-ов, я пока хз), преобразованные в постфиксную запись для быстрого вычисления
struct Expression {
  vector<Item> content;
};

struct ParseExpressionReturned {
  Expression source;
  Exception exception = Exception(Nothing);

  ParseExpressionReturned(Expression s, Exception e) : source(s), exception(e) {}

  ParseExpressionReturned(Expression s) : source(s), exception(Nothing) {}

  ParseExpressionReturned(Exception e) : exception(e) {}
};

ParseExpressionReturned parseExpression(const vector<Token> &input, int &index);

struct CalculateReturned {
  Item item;
  Exception exception = Exception(Nothing);

  CalculateReturned(Item item, Exception e = Nothing) : item(item), exception(e) {}

  CalculateReturned(Exception e) : item(""), exception(e) {}
};

CalculateReturned Calculate(Expression expression);

struct ConstructorReturned {
  Item item;
  Exception exception;

  ConstructorReturned(Item i, Exception e) : item(i), exception(e) {}

  ConstructorReturned(Item i) : item(i), exception(Nothing) {}

  ConstructorReturned(Exception e) : item(""), exception(e) {}
};

// 'a' -> a, "a" -> a
string formatString(string inp) {
	string ret;
	for (int i = 1; i < inp.size() - 1; ++i) {
		ret += inp[i];
	}
	return ret;
}

vector<Item> getContent(Item item) {
	if (item.type == "Array") {
		return static_cast<Array *>(item.value)->content;
	}
	if (item.type == "MutableArray") {
		return static_cast<MutableArray *>(item.value)->content;
	}
	if (item.type == "ArrayList") {
		return static_cast<ArrayList *>(item.value)->content;
	}
	if (item.type == "MutableArrayList") {
		return static_cast<MutableArrayList *>(item.value)->content;
	}
	if (item.type == "List") {
		list<Item> l = static_cast<List *>(item.value)->content;
		vector<Item> ret;
		for (Item i : l) {
			ret.push_back(i);
		}
		return ret;
	}
	if (item.type == "MutableList") {
		list<Item> l = static_cast<MutableList *>(item.value)->content;
		vector<Item> ret;
		for (Item i : l) {
			ret.push_back(i);
		}
		return ret;
	}
	if (item.type == "Set") {
		set<Item> s = static_cast<Set *>(item.value)->content;
		vector<Item> ret;
		for (Item i : s) {
			ret.push_back(i);
		}
		return ret;
	}
	if (item.type == "MutableSet") {
		set<Item> s = static_cast<MutableSet *>(item.value)->content;
		vector<Item> ret;
		for (Item i : s) {
			ret.push_back(i);
		}
		return ret;
	}
}


ConstructorReturned callConstructor(string className, vector<Item> &args) {
	if (contain({"Object", "Container", "Any", "Unit"}, className)) {
		return Exception(ConstructorCallError);
	}
	if (args.size() == 1) {
		if (args[0].type == className) {
			return args[0];
		}
	}
	if (className == "Int") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		if (args.size() == 0) {
			return Item(static_cast<void *>(new Int(0)), "Int");
		}
		Item arg = args[0];
		if (arg.type == "String") {
			string formatted = formatString(arg.source);
			if (getToken(formatted).type == IntNumber) {
				return Item(static_cast<void *>(new Int(stoll(formatted))), "Int");
			} else {
				return Exception(ValueError);
			}
		}
		if (arg.type == "Float") {
			Float v = *static_cast<Float *>(arg.value);
			return Item(static_cast<void *>(new Int(v.value)), "Int");
		}
		return Exception(ConstructorCallError);
	}
	if (className == "Float") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		if (args.size() == 0) {
			return Item(static_cast<void *>(new Float(0)), "Float");
		}
		Item arg = args[0];
		if (arg.type == "String") {
			string formatted = formatString(arg.source);
			if (arg.token.type == FloatNumber) {
				return Item(static_cast<void *>(new Float(stold(formatted))), "Float");
			} else {
				return Exception(ValueError);
			}
		}
		if (arg.type == "Int") {
			Int v = *static_cast<Int *>(arg.value);
			return Item(static_cast<void *>(new Float(v.value)), "Float");
		}
		return Exception(ConstructorCallError);
	}
	if (className == "String") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		if (args.size() == 0) {
			return Item(static_cast<void *>(new String("")), "String");
		}
		Item arg = args[0];
		if (arg.type == "Int") {
			Int v = *static_cast<Int *>(arg.value);
			return Item(static_cast<void *>(new String(to_string(v.value))), "String");
		}
		if (arg.type == "Float") {
			Float v = *static_cast<Float *>(arg.value);
			string tmp = to_string(v.value);
			return Item(static_cast<void *>(new String(formatFloatNumber(tmp))), "String");
		}
		if (arg.type == "Char") {
			Char v = *static_cast<Char *>(arg.value);
			return Item(static_cast<void *>(new String(string(1, v.value))), "String");
		}
		if (arg.type == "Bool") {
			Bool v = *static_cast<Bool *>(arg.value);
			return Item(static_cast<void *>(new String(v.value ? "True" : "False")), "String");
		}
		if (arg.type == "Range") {
			Range r = *static_cast<Range *>(arg.value);
			return Item(static_cast<void *>(new String(
				"Range : { left : " + formatFloatNumber(to_string(r.left.value)) +
				", right : " + formatFloatNumber(to_string(r.right.value)) +
				", step : " + formatFloatNumber(to_string(r.step.value)) + " }"
			)), "String");
		}
		if (arg.type == "Function") {
			return Item(static_cast<void *>(new String("Function")), "String");
		}
		if (arg.type == "NoneType") {
			return Item(static_cast<void *>(new String("None")), "String");
		}
		if (arg.type == "Array") {
			Array v = *static_cast<Array *>(arg.value);
			string s = "[";
			int i = 0;
			for (Item item : v.content) {
				vector<Item> t = vector<Item>(1, item);
				auto result = callConstructor("String", t);
				if (result.exception.type != Nothing) {
					return result.exception;
				}
				s += static_cast<String *> (result.item.value)->value + (i + 1 == v.content.size() ? "]" : ", ");
				++i;
			}
			return Item(static_cast<void *>(new String(s)), "String");
		}
		if (arg.type == "MutableArray") {
			MutableArray v = *static_cast<MutableArray *>(arg.value);
			string s = "[";
			int i = 0;
			for (Item item : v.content) {
				vector<Item> t = vector<Item>(1, item);
				auto result = callConstructor("String", t);
				if (result.exception.type != Nothing) {
					return result.exception;
				}
				s += static_cast<String *> (result.item.value)->value + (i + 1 == v.content.size() ? "]" : ", ");
				++i;
			}
			return Item(static_cast<void *>(new String(s)), "String");
		}
		if (arg.type == "ArrayList") {
			ArrayList v = *static_cast<ArrayList *>(arg.value);
			string s = "[";
			int i = 0;
			for (Item item : v.content) {
				vector<Item> t = vector<Item>(1, item);
				auto result = callConstructor("String", t);
				if (result.exception.type != Nothing) {
					return result.exception;
				}
				s += static_cast<String *> (result.item.value)->value + (i + 1 == v.content.size() ? "]" : ", ");
				++i;
			}
			return Item(static_cast<void *>(new String(s)), "String");
		}
		if (arg.type == "MutableArray") {
			MutableArrayList v = *static_cast<MutableArrayList *>(arg.value);
			string s = "[";
			int i = 0;
			for (Item item : v.content) {
				vector<Item> t = vector<Item>(1, item);
				auto result = callConstructor("String", t);
				if (result.exception.type != Nothing) {
					return result.exception;
				}
				s += static_cast<String *> (result.item.value)->value + (i + 1 == v.content.size() ? "]" : ", ");
				++i;
			}
			return Item(static_cast<void *>(new String(s)), "String");
		}
		if (arg.type == "List") {
			List v = *static_cast<List *>(arg.value);
			string s = "[";
			int i = 0;
			for (Item item : v.content) {
				vector<Item> t = vector<Item>(1, item);
				auto result = callConstructor("String", t);
				if (result.exception.type != Nothing) {
					return result.exception;
				}
				s += static_cast<String *> (result.item.value)->value + (i + 1 == v.content.size() ? "]" : ", ");
				++i;
			}
			return Item(static_cast<void *>(new String(s)), "String");
		}
		if (arg.type == "MutableList") {
			Array v = *static_cast<Array *>(arg.value);
			string s = "[";
			int i = 0;
			for (Item item : v.content) {
				vector<Item> t = vector<Item>(1, item);
				auto result = callConstructor("String", t);
				if (result.exception.type != Nothing) {
					return result.exception;
				}
				s += static_cast<String *> (result.item.value)->value + (i + 1 == v.content.size() ? "]" : ", ");
				++i;
			}
			return Item(static_cast<void *>(new String(s)), "String");
		}
		if (arg.type == "Set") {
			Set v = *static_cast<Set *>(arg.value);
			string s = "[";
			int i = 0;
			for (Item item : v.content) {
				vector<Item> t = vector<Item>(1, item);
				auto result = callConstructor("String", t);
				if (result.exception.type != Nothing) {
					return result.exception;
				}
				s += static_cast<String *> (result.item.value)->value + (i + 1 == v.content.size() ? "]" : ", ");
				++i;
			}
			return Item(static_cast<void *>(new String(s)), "String");
		}
		if (arg.type == "MutableSet") {
			MutableSet v = *static_cast<MutableSet *>(arg.value);
			string s = "[";
			int i = 0;
			for (Item item : v.content) {
				vector<Item> t = vector<Item>(1, item);
				auto result = callConstructor("String", t);
				if (result.exception.type != Nothing) {
					return result.exception;
				}
				s += static_cast<String *> (result.item.value)->value + (i + 1 == v.content.size() ? "]" : ", ");
				++i;
			}
			return Item(static_cast<void *>(new String(s)), "String");
		}
		return Item(static_cast<void *>(new String(arg.type)), "String");
	}
	if (className == "Char") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		if (args.size() == 0) {
			return Item(static_cast<void *>(new Char(' ')), "Char");
		}
		return Exception(ConstructorCallError);
	}
	if (className == "Bool") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		if (args.size() == 0) {
			return Item(static_cast<void *>(new Bool(false)), "Bool");
		}
		return Exception(ConstructorCallError);
	}
	if (className == "Range") {
		if (args.size() == 0) {
			return Exception(FunctionArgumentLack);
		}
		if (args.size() > 3) {
			return Exception(FunctionArgumentExcess);
		}
		for (Item item : args) {
			if (item.type != "Float" and item.type != "Int") {
				return Exception(TypeError);
			}
		}
		Float arg0 = Float(
			args[0].type == "Int" ? *static_cast<Int *>(args[0].value) : *static_cast<Float *>(args[0].value));
		if (args.size() == 1) {
			return Item(static_cast<void *>(new Range(arg0)), "Range");
		}
		Float arg1 = Float(
			args[1].type == "Int" ? *static_cast<Int *>(args[1].value) : *static_cast<Float *>(args[1].value));
		if (args.size() == 2) {
			return Item(static_cast<void *>(new Range(arg0, arg1)), "Range");
		}
		Float arg2 = Float(
			args[2].type == "Int" ? *static_cast<Int *>(args[2].value) : *static_cast<Float *>(args[2].value));
		return Item(static_cast<void *>(new Range(arg0, arg1, arg2)), "Range");
	}
	if (className == "Function") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "NoneType") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "Array") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "MutableArray") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "ArrayList") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "MutableArrayList") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "List") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "MutableList") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "Stack") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "Queue") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "Deque") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "Set") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "MutableSet") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "Map") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	if (className == "MutableMap") {
		if (args.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Exception(ConstructorCallError);
	}
	return Exception(UnknownTypeError);
}


ConstructorReturned parseConstructorCall(const vector<Token> &input, int &index) {
	if (Classes.find(input[index].source) == Classes.end() and
	    BuiltInClasses.find(input[index].source) == BuiltInClasses.end()) {
		return Exception(ConstructorCallError, getLineIndex(input, index));
	}
	string className = input[index].source;
	int constructorCallIndex = getLineIndex(input, index);
	index = nextIndex(input, index);
	if (input[index].source != "(") {
		return Exception(ConstructorCallError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	vector<Item> args;
	while (input[index].source != ")") {
		auto result = parseExpression(input, index);
		if (result.exception.type != Nothing) {
			return Exception(result.exception.type, getLineIndex(input, index));
		}
		CalculateReturned calculateReturned = Calculate(result.source);
		if (calculateReturned.exception.type != Nothing) {
			return Exception(calculateReturned.exception.type, getLineIndex(input, index));
		}
		args.push_back(calculateReturned.item);
		if (not contain({")", ","}, input[index].source)) {
			return Exception(ConstructorCallError, getLineIndex(input, index));
		}
		if (input[index].source == ",") {
			index = nextIndex(input, index);
		}
	}
	index = nextIndex(input, index);
	return callConstructor(className, args);
}

#endif //NEKO_INTERPRETER_OBJECT_HPP