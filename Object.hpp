#ifndef NEKO_INTERPRETER_OBJECT_HPP
#define NEKO_INTERPRETER_OBJECT_HPP

#include "helpers.hpp"
#include "exceptions.hpp"

class Object {
  public:

  void *value;

  virtual void toBool() {};

  virtual void toString() {};

  virtual void toInt() {};

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
};

class Int : Object {
  public:
  long long value;

  Int(long long init) : value(init) {}
};

class Float : Object {
  public:
  long double value;

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

struct Function : Object {
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
			  return "FunctionObject";
	  }
	  assert(false);
  }
};

bool operator>(Item &item1, Item &item2) {
	return item1.value > item2.value;
}

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
  virtual Int getLength() {
	  return Int(length);
  }

  virtual Exception add(Item item) {};

  virtual bool isEmpty() {
	  return length == 0;
  }

};

class Array : Container {
  private:
  int length;
  public:
  vector<Item> content;

  Array(int n, Item init = Item("")) : length(0) {}

  Int getLength() {
	  return Int(length);
  }

  virtual ContainerReturned get(int index) {
	  if (index < 0 or index >= this->length) {
		  return Exception(IndexError);
	  }
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
	  return
  }

  Exception clear() {
	  while (not content.empty()) {
		  content.pop();
	  }
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

};

class List : Container {
  private:
  int length;
  public:
  list<Item> content;
};

class MutableList : List {
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

};

class MutableSet : Set {
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

};

Exception parseInitializerList(vector<Item> &input, int &index) {

}

Exception parseMethodCall() {

}

Exception callMethod() {

}

Exception callConstructor(string className, vector<Item> &input) {
	if (contain({"Object", "Container", "Any", "Unit"}, className)) {
		return Exception(ConstructorCallError);
	}
	if (className == "Int") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Float") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "String") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Char") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Bool") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Function") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "NoneType") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Array") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "ArrayList") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "List") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "MutableList") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Stack") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Queue") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Deque") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Set") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "MutableSet") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "Map") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	if (className == "MutableMap") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
	}
	return Exception(UnknownTypeError);
}

#endif //NEKO_INTERPRETER_OBJECT_HPP