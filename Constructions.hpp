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

// TODO: убрать скобки при if, elif, while

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

Exception parseIfStatement(const vector<Token> &input, int &index) {
	if (input[index].source != "if") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	bool used = false, logicValue;
	do {
		if (contain({"if", "elif"}, input[index].source) or
		    (input[index].source == "else" and input[nextIndex(input, index)].source == "if")) {
			if (input[index].source == "else" and input[nextIndex(input, index)].source == "if") {
				index = nextIndex(input, index);
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
			if (item.type != "Bool") {
				return Exception(TypeError, getLineIndex(input, index));
			}
			logicValue = static_cast<Bool *>(item.value)->value;
		} else logicValue = true;
		vector<Token> ifBody;
		index = nextIndex(input, index);
		if (input[index].source != "{") {
			return Exception(SyntaxError, getLineIndex(input, index));
		}
		index = nextIndex(input, index);
		stack<string> bracketStack;
		while (not(bracketStack.empty() and input[index].source == "}")) {
			if (input[index].source == "{") {
				bracketStack.push(input[index].source);
			}
			if (input[index].source == "}") {
				bracketStack.pop();
			}
			if (logicValue and not used) {
				ifBody.push_back(input[index]);
			}
			index++;
		}
		index = nextIndex(input, index);
		if (logicValue and not used) {
			executeReturned result = execute(ifBody);
			if (result.exception.type != Nothing) {
				return result.exception;
			}
			used = true;
		}
	} while (contain({"else", "elif"}, input[index].source));

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
	if (item.type != "Bool") {
		return Exception(TypeError, getLineIndex(input, index));
	}
	bool logicValue = static_cast<Bool *>(item.value)->value;
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
		index++;
	}
	index = nextIndex(input, index);
	const int endIndex = index;
	while (logicValue) {
		executeReturned result = execute(whileBody);
		if (not contain({Nothing, CONTINUE}, result.exception.type)) {
			if (result.exception.type == BREAK) {
				break;
			}
			return result.exception;
		}
		index = conditionIndex;
		logicValue = static_cast<Bool *>(Calculate(parseExpression(input, index).source).item.value)->value;
	}
	index = endIndex;
	return Nothing;
}

// TODO: add parseForStatement
Exception parseForStatement(const vector<Token> &input, int &index) {
	if (input[index].source != "for") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	if (input[index].source != "(") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	if (input[index].type != Name) {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
}

ternaryReturned parseTernary(const vector<Token> &input, int &index) {
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
	if (item.type != "Bool") {
		return Exception(TypeError, getLineIndex(input, index));
	}
	bool logicValue = static_cast<Bool *>(item.value)->value;
	index = nextIndex(input, index);
	parseResult = parseExpression(input, index);
	if (parseResult.exception.type != Nothing) {
		return Exception(parseResult.exception.type, getLineIndex(input, index));
	}
	if (logicValue) {
		calculationResult = Calculate(parseResult.source);
		if (calculationResult.exception.type != Nothing) {
			return Exception(calculationResult.exception.type, getLineIndex(input, index));
		}
	}
	Item item1 = calculationResult.item;
	if (input[index].source != "else") {
		return Exception(SyntaxError, getLineIndex(input, index));
	}
	index = nextIndex(input, index);
	parseResult = parseExpression(input, index);
	if (parseResult.exception.type != Nothing) {
		return Exception(parseResult.exception.type, getLineIndex(input, index));
	}
	if (not logicValue) {
		calculationResult = Calculate(parseResult.source);
		if (calculationResult.exception.type != Nothing) {
			return Exception(calculationResult.exception.type, getLineIndex(input, index));
		}
	}
	Item item2 = calculationResult.item;
	return logicValue ? item1 : item2;
}



#endif //NEKO_INTERPRETER_CONSTRUCTIONS_HPP
