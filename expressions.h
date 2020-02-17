#ifndef NEKO_INTERPRETER_EXPRESSIONS_H
#define NEKO_INTERPRETER_EXPRESSIONS_H

#include "token.h"
#include "exceptions.h"
#include <variant>
#include "Constructions.h"

vector<Token> intoPostfixNotation(const vector<Token> &input, int &index) {
	stack<Token> variables, operations;
	vector<Token> output;
	int end = input.size();
	for (int i = index; i < end - 1; ++i) {
		Token token = input[i];
		if (token.isOperator()) {

		}
	}
}

// последовательности токенов (или item-ов, я пока хз), преобразованные в постфиксную запись для быстрого вычисления
struct Expression : BasicConstruction {
  Expression() {
	  type = ExpressionConstruction;
  }

  vector<variant < ConditionalConstruction, WhileConstruction, ForConstruction, Expression>> content;
};

struct ParseExpressionReturned {
  Expression source;
  Exception exception = Exception(Nothing);
};

ParseExpressionReturned parseExpression(vector<Token> input, int &index) {

}

#endif //NEKO_INTERPRETER_EXPRESSIONS_H
