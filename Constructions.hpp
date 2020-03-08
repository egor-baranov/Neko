#ifndef NEKO_INTERPRETER_CONSTRUCTIONS_HPP
#define NEKO_INTERPRETER_CONSTRUCTIONS_HPP

#include "expressions.hpp"
#include "execution.hpp"

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

Exception parseIfStatement(const vector<Token> &input, int &index) {
	if (input[index].source != "if") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	if (input[index].source != "(") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	auto parseResult = parseExpression(input, index);
	if (parseResult.exception.type != Nothing) {
		return Exception(parseResult.exception.type, getLineIndex(input, index));
	}
	auto calculationResult = Calculate(parseResult.source);
	if (calculationResult.exception.type != Nothing) {
		return Exception(calculationResult.exception.type, getLineIndex(input, index));
	}
	Item item = calculationResult.item;
	if (item.type != BoolType) {
		return Exception(TypeError, getLineIndex(input, index));
	}
	bool logicValue = *static_cast<bool *>(item.value);
	vector<Token> ifBody;
	index = nextIndex(input, index);
	if (input[index].source != "{") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	stack<string> bracketStack;
	while (!(bracketStack.empty() and input[index].source == "}")) {
		if (input[index].source == "{") {
			bracketStack.push(input[index].source);
		}
		if (input[index].source == "}") {
			bracketStack.pop();
		}
		if (logicValue) {
			ifBody.push_back(input[index]);
		}
		index = nextIndex(input, index);
	}
	index = nextIndex(input, index);
	if (logicValue) {
		return execute(ifBody);
	}
	return Exception(Nothing);
}

Exception parseWhileStatement(const vector<Token> &input, int &index) {
	if (input[index].source != "while") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	if (input[index].source != "(") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	// TODO: сохранить как последовательность токенов, чтобы переиспользовать
	const int conditionIndex = index;
	auto parseResult = parseExpression(input, index);
	if (parseResult.exception.type != Nothing) {
		return Exception(parseResult.exception.type, getLineIndex(input, index));
	}
	auto calculationResult = Calculate(parseResult.source);
	if (calculationResult.exception.type != Nothing) {
		return Exception(calculationResult.exception.type, getLineIndex(input, index));
	}
	Item item = calculationResult.item;
	if (item.type != BoolType) {
		return Exception(TypeError, getLineIndex(input, index));
	}
	bool logicValue = *static_cast<bool *>(item.value);
	vector<Token> whileBody;
	index = nextIndex(input, index);
	if (input[index].source != "{") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	stack<string> bracketStack;
	while (!(bracketStack.empty() and input[index].source == "}")) {
		if (input[index].source == "{") {
			bracketStack.push(input[index].source);
		}
		if (input[index].source == "}") {
			bracketStack.pop();
		}
		if (logicValue) {
			whileBody.push_back(input[index]);
		}
		index = nextIndex(input, index);
	}
	index = nextIndex(input, index);
	while (logicValue) {
		Exception exception = execute(whileBody);
		if (exception.type != Nothing) {
			return exception;
		}
		// TODO: чекаем calculate от этой самой последовательности и тд
		index = conditionIndex;
		logicValue = *static_cast<bool *>(Calculate(parseExpression(input, index).source).item.value);
	}
	return Nothing;
}

#endif //NEKO_INTERPRETER_CONSTRUCTIONS_HPP
