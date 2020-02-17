#ifndef NEKO_INTERPRETER_CONSTRUCTIONS_H
#define NEKO_INTERPRETER_CONSTRUCTIONS_H

enum ConstructionType {
  Conditional,
  While,
  For,
  ExpressionConstruction
};

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

#endif //NEKO_INTERPRETER_CONSTRUCTIONS_H
