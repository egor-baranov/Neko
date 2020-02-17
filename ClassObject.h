#ifndef NEKO_INTERPRETER_CLASSOBJECT_H
#define NEKO_INTERPRETER_CLASSOBJECT_H

#include "token.h"
#include "exceptions.h"
#include "VariableObject.h"
#include "FunctionObject.h"

struct ClassObject {
  string name = "";
  string parentName = "Object";
};

bool operator>(ClassObject a, ClassObject b) {
	return a.name > b.name;
}

bool operator<(ClassObject a, ClassObject b) {
	return a.name < b.name;
}

bool operator==(ClassObject a, ClassObject b) {
	return a.name == b.name;
}

map<string, ClassObject> Classes;

Exception parseClassDeclaration(const vector<Token> &input, int &index) {
	ClassObject classObject = ClassObject();
	Classes[classObject.name] = classObject;
	return Exception(Nothing);
}

#endif //NEKO_INTERPRETER_CLASSOBJECT_H
