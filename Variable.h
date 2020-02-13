#ifndef NEKO_INTERPRETER_VARIABLE_H
#define NEKO_INTERPRETER_VARIABLE_H

#include "token.h"
#include "exceptions.h"

struct VariableObject {
  bool isMutable = false;
  string name = "";
  string typeName = "Any";
};

bool operator>(VariableObject a, VariableObject b) {
  return a.name > b.name;
}

bool operator<(VariableObject a, VariableObject b) {
  return a.name < b.name;
}

map<string, VariableObject> Variables;

Exception parseVariableDeclaration(const vector<Token> &input, int &index) {
  if (input.size() < 3) {
    return Exception(VariableDeclarationError, getLineIndex(input, index));
  }
  VariableObject variable;
  if (contain({"var", "val"}, input[index].source)) {
    variable.isMutable = input[index].source == "var";
    index = nextIndex(input, index);
  }
  if (input[index].type != Name) {
    return Exception(VariableDeclarationError, getLineIndex(input, index));
  }
  variable.name = input[index].source;
  index = nextIndex(input, index);
  if (not contain({"=", ":"}, input[index].source)) {
    return Exception(VariableDeclarationError, getLineIndex(input, index));
  }
  if (input[index].source == ":") {
    index = nextIndex(input, index);
    if (not contain({Name}, input[index].type)) {
      return Exception(VariableDeclarationError, getLineIndex(input, index));
    }
    variable.typeName = input[index].source;
    index = nextIndex(input, index);
  }
  if (input[index].source != "=") {
    return Exception(VariableDeclarationError, getLineIndex(input, index));
  }
  Variables[variable.name] = variable;
  return Exception(Nothing);
}


#endif //NEKO_INTERPRETER_VARIABLE_H
