#ifndef NEKO_INTERPRETER_CONSTRUCTIONS_H
#define NEKO_INTERPRETER_CONSTRUCTIONS_H

#include "expressions.h"

enum ConstructionType {
  Conditional,
  While,
  For,
  ExpressionConstruction,
  InitializerList
};

string toString(ConstructionType input) {
	switch (input) {
		case Conditional:
			return "Conditional";
		case While:
			return "While";
		case For:
			return "For";
		case ExpressionConstruction:
			return "ExpressionConstruction";
		case InitializerList:
			return "InitializerList";
	}
}

struct BasicConstruction {
  ConstructionType type;
};

struct ConditionalConstruction : BasicConstruction {
  ConditionalConstruction() {
	  type = Conditional;
  }

};

struct WhileConstruction : BasicConstruction {
  WhileConstruction() {
	  type = While;
  }
};

struct ForConstruction : BasicConstruction {
  ForConstruction() {
	  type = For;
  }
};

struct InitializerListConstruction : BasicConstruction {
  InitializerListConstruction() {

  }
};

struct Scope {
  int scopeId;
  // {ConditionalConstruction, WhileConstruction, ForConstruction, Expression} которые наследуются от BasicConstruction
  vector<BasicConstruction *> content;

  Scope() {
	  scopeId = std::rand();
  }

  void closeScope() {

  }
};

#endif //NEKO_INTERPRETER_CONSTRUCTIONS_H
