#ifndef NEKO_INTERPRETER_OBJECT_HPP
#define NEKO_INTERPRETER_OBJECT_HPP

#include "helpers.hpp"
#include "exceptions.hpp"

class Object {
  void *value;

  void toString() {
  }
};

enum IntValueType {

};

class Int : Object {

  void toString() {

  }
};

enum FloatValueType {

};

class Float : Object {

};

class Char : Object {

};

class String : Object {

};

class Function : Object {

};

#endif //NEKO_INTERPRETER_OBJECT_HPP
