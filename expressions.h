#ifndef NEKO_INTERPRETER_EXPRESSIONS_H
#define NEKO_INTERPRETER_EXPRESSIONS_H

#include "token.h"
#include "exceptions.h"
#include <variant>

void *getValue(const Token &token) {
	if (token.type == StringLiteral) {
		string ret = sliceString(token.source, 1, token.source.size() - 2);
		return static_cast<void *> (new string(ret));
	}
	if (token.type == CharLiteral) {
		return static_cast<void *>(new char(token.source[1]));
	}
	if (token.type == IntNumber) {
		return static_cast<void *>(new int(stoi(token.source)));
	}
	if (token.type == FloatNumber) {
		return static_cast<void *>(new double(stod(token.source)));
	}
}

struct Item {
  Token token = Token(None, "");
  void *value;

  Item(Token init) {
	  token = init;
	  value = getValue(init);
  }

  string toString() {
	  if (token.type == StringLiteral) {
		  return *static_cast<string *>(value);
	  }
	  if (token.type == CharLiteral) {
		  return string(static_cast<char *>(value));
	  }
	  if (token.type == IntNumber) {
		  return to_string(*static_cast<int *>(value));
	  }
	  if (token.type == FloatNumber) {
		  return to_string(*static_cast<double *>(value));
	  }
  }
};

vector<Token> intoPostfixNotation(vector<Item> input) {
	stack<Token> variables, operations;
	vector<Item> output;
	int end = input.size();
	for (int i = 0; i < end; ++i) {
		Token token = input[i].token;
		if (token.isEndOfExpression()) {
			continue;
		}
		if (token.isOperator()) {
			stack<Token> opTmp;
			while (not operations.empty()) {
				if (operations.top().source == "(" or getPriority(operations.top()) < getPriority(token)) {
					break;
				}
				opTmp.push(operations.top());
				operations.pop();
			}
			operations.push(token);
			while (not opTmp.empty()) {
				operations.push(opTmp.top());
				opTmp.pop();
			}
		} else {
			if (token.source == ")") {

			} else {
				output.push_back(token);
			}
		}
	}
}

// последовательности токенов (или item-ов, я пока хз), преобразованные в постфиксную запись для быстрого вычисления
struct Expression {
  vector<Item> content;
};


void *Process(Item &a, Item &b, Token op) {

}

Item Calculate(Expression expression) {
	if (expression.content.size() == 1) {
		return expression.content[0];
	}
	// вычисление выражений
	for (auto elem: intoPostfixNotation(expression.content)) {

	}
	return Item(emptyToken);
}

struct ParseExpressionReturned {
  Expression source;
  Exception exception = Exception(Nothing);
};

ParseExpressionReturned parseExpression(const vector<Token> &input, int &index) {
	Token token = input[index];
	int end = input.size() - 1;
	Expression ret;
	while (index < end) {
		ret.content.push_back(Item(input[index]));
		index = nextIndex(input, index);
		break;
	}
	return {ret, Exception(Nothing)};
}

#endif //NEKO_INTERPRETER_EXPRESSIONS_H
