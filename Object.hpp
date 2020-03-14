#ifndef NEKO_INTERPRETER_OBJECT_HPP
#define NEKO_INTERPRETER_OBJECT_HPP

#include "helpers.hpp"
#include "exceptions.hpp"

class Object {
  public:
  void *value;

  virtual void toBool() {

  }

  virtual void toString() {

  }

  virtual void serialize() {

  }

  virtual void call() {

  }

  virtual void index() {

  }

  virtual void contain(Object other) {

  }

  virtual void slice() {

  }

  virtual void iterate() {

  }

  virtual void bit() {

  }
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


#endif //NEKO_INTERPRETER_OBJECT_HPP