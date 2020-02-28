#ifndef NEKO_INTERPRETER_CLASSOBJECT_HPP
#define NEKO_INTERPRETER_CLASSOBJECT_HPP

#include "token.hpp"
#include "exceptions.hpp"
#include "VariableObject.hpp"
#include "FunctionObject.hpp"

bool operator>(ClassObject a, ClassObject b) {
	return a.name > b.name;
}

bool operator<(ClassObject a, ClassObject b) {
	return a.name < b.name;
}

bool operator==(ClassObject a, ClassObject b) {
	return a.name == b.name;
}

Exception parseClassDeclaration(const vector<Token> &input, int &index) {
	ClassObject classObject = ClassObject();
	Classes[classObject.name] = classObject;
	return Exception(Nothing);
}

#endif //NEKO_INTERPRETER_CLASSOBJECT_HPP
