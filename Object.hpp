#ifndef NEKO_INTERPRETER_OBJECT_HPP
#define NEKO_INTERPRETER_OBJECT_HPP

#include "helpers.hpp"
#include "exceptions.hpp"

class Object {
  void *value;

  void toString() {
  }
};


class Int : Object {
  int value;

  string toString() {

  }

};

class Float : Object {
  double value;
};

class Char : Object {

};

class String : Object {

};

#endif //NEKO_INTERPRETER_OBJECT_HPP
