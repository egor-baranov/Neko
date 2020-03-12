#ifndef NEKO_INTERPRETER_OBJECT_HPP
#define NEKO_INTERPRETER_OBJECT_HPP

#include "helpers.hpp"
#include "exceptions.hpp"

class Object {
  void *value;

  void toBool() {

  }

  void toString() {
  }

  void serialize() {

  }

  void call() {

  }

  void index() {

  }

  void contain(Object other) {

  }

  void slice() {

  }

  void iterate() {

  }

  void bit() {

  }
};


class Int : Object {
  int value;

};

class Float : Object {
  double value;
};

class Char : Object {

};

class String : Object {

};

#endif //NEKO_INTERPRETER_OBJECT_HPP
