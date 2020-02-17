#ifndef NEKO_INTERPRETER_EXCEPTIONS_H
#define NEKO_INTERPRETER_EXCEPTIONS_H

#include <bits/stdc++.h>
#include "token.h"

using namespace std;

enum exType {
  UndefinedNameUsage,
  VariableDeclarationError,
  FunctionDeclarationError,
  AssignmentError, // попытка присвоить значение к константе, а также некорректная инициализация
  OperatorPriorityError, // a *+^ b
  OperatorSequenceError, // = += *= 3
  TooManyPointsInNumber,
  MemberError, // объект не содержит поля, метода и тд
  SyntaxError,
  UnexpectedDotError, // 234 .234 или f .do() и тд
  CharFormatError, // длина символьного литерала превосходит один символ
  UnterminatedComment,
  ZeroDivisionError,
  NameError,
  BracketSequenceError,
  QuotesSequenceError,
  ElementSequenceError,
  FunctionCallError,
  FunctionArgumentExcess,
  FunctionArgumentLack,
  IncorrectOperationArguments,
  InputError,
  MathError,
  MemoryError,
  RuntimeError,
  UnexpectedTokenError,
  Nothing
};

string toString(exType type) {
	switch (type) {
		case UndefinedNameUsage:
			return "UndefinedNameUsage";
		case VariableDeclarationError:
			return "VariableDeclarationError";
		case FunctionDeclarationError:
			return "FunctionDeclarationError";
		case AssignmentError:
			return "AssignmentError";
		case OperatorPriorityError:
			return "OperatorPriorityError";
		case OperatorSequenceError:
			return "OperatorSequenceError";
		case TooManyPointsInNumber:
			return "TooManyPointsInNumber";
		case MemberError:
			return "MemberError";
		case SyntaxError:
			return "SyntaxError";
		case UnexpectedDotError:
			return "UnexpectedDotError";
		case CharFormatError:
			return "CharFormatError";
		case UnterminatedComment:
			return "UnterminatedComment";
		case ZeroDivisionError:
			return "ZeroDivisionError";
		case NameError:
			return "NameError";
		case BracketSequenceError:
			return "BracketSequenceError";
		case QuotesSequenceError:
			return "QuotesSequenceError";
		case ElementSequenceError:
			return "ElementSequenceError";
		case FunctionCallError:
			return "FunctionCallError";
		case FunctionArgumentExcess:
			return "FunctionArgumentExcess";
		case FunctionArgumentLack:
			return "FunctionArgumentLack";
		case IncorrectOperationArguments:
			return "IncorrectOperationArguments";
		case UnexpectedTokenError:
			return "UnexpectedTokenError";
		case InputError:
			return "InputError";
		case MathError:
			return "MathError";
		case MemoryError:
			return "MemoryError";
		case RuntimeError:
			return "RuntimeError";
		case Nothing:
			return "Nothing";
	}
}

struct Exception {
  exType type;
  int line;

  Exception(exType eT) {
	  type = eT;
	  line = -1;
  }

  Exception(exType eT, int l) : type(eT), line(l) {}
};

string errorMessage(Exception ex) {
	return toString(ex.type) + " in line " + to_string(ex.line);
}

void throwException(Exception ex) {
	cerr << errorMessage(ex) << endl;
}

Exception syntaxErrorAnalysis(vector<Token> input) {
	int lineIndex = 1;
	stack<Token> bracketStack;
	bool isString = false, isChar = false;
	// длина входного потока
	int end = input.size();
	// последний символ - EOfF, поэтому читаем до предпоследнего
	for (int i = 0; i < end - 1; ++i) {
		lineIndex = getLineIndex(input, i);
		Token token = input[i];
		Token prevToken = prev(input, i), nextToken = next(input, i);

		if (token.isEndOfLine()) {
			if (isString or isChar) {
				return Exception(QuotesSequenceError, lineIndex);
			}
			continue;
		}
		// обработка строк
		if (token.source == "\"" and not isChar) {
			isString = not isString;
			continue;
		}
		if (token.source == "\'" and not isString) {
			isChar = not isChar;
			continue;
		}
		// обработка скобочных последовательностей
		if (token.isBracket()) {
			if (token.isLeftBracket()) {
				bracketStack.push(token);
			} else {
				if (bracketStack.empty()) {
					return Exception(BracketSequenceError, lineIndex);
				} else {
					Token tmp = bracketStack.top();
					bracketStack.pop();
					if (not isBracketPair(tmp, token))
						return Exception(BracketSequenceError, lineIndex);
				}
			}
			continue;
		}
		// обработка CharFormatError
		if (token.type == CharLiteral and token.source.size() != 3)
			return Exception(CharFormatError, lineIndex);
		// обработка неправильных имен
		if (token.type == Name and not isCorrectName(token.source)) {
			if (token.source[0] == '\"' or token.source[0] == '\'')
				return Exception(QuotesSequenceError, lineIndex);
			return Exception(NameError, lineIndex);
		}
		// MemberError для Int после Name
		// а также UnexpectedDotError - один из операндов точки ключевое слово или оператор и тд
		if (token.source == ".") {
			if (prevToken.source == "." or nextToken.source == ".") continue;
			if (prevToken.type != IntNumber and nextToken.type == IntNumber) {
				return Exception(MemberError, lineIndex);
			}
			if (token.source == "." and i == 0) {
				return Exception(UnexpectedDotError, lineIndex);
			}
			if (prevToken.type == Keyword or prevToken.isOperator() or prevToken.type == Constant or
			    nextToken.type == Keyword or nextToken.isOperator() or nextToken.type == Constant) {
				if (prevToken.source != "this")
					return Exception(UnexpectedDotError, lineIndex);
			}
			if (not prevToken.isRightBracket() and prevToken.type == Punctuation) {
				return Exception(UnexpectedDotError, lineIndex);
			}
			if (not nextToken.isLeftBracket() and nextToken.type == Punctuation) {
				return Exception(UnexpectedDotError, lineIndex);
			}
		}
		// обработка неправильного использования оператора .  - TooManyPointsInNumber
		if (token.type == IntNumber and i > 1 and i < end - 2) {
			if (input[i - 2].type == IntNumber and input[i - 1].source == "." and
			    input[i + 1].source == "." and input[i + 2].type == IntNumber) {
				return Exception(TooManyPointsInNumber, lineIndex);
			}
		}
		// обработка UnexpectedTokenError
		if (token.source == ",") {
			if (i == 0) return Exception(UnexpectedTokenError, lineIndex);

			if (not prevToken.isRightBracket() and prevToken.type == Punctuation) {
				return Exception(UnexpectedTokenError, lineIndex);
			}
			if (not nextToken.isLeftBracket() and nextToken.type == Punctuation) {
				return Exception(UnexpectedTokenError, lineIndex);
			}

			if (not contain({"+", "-"}, prevToken.source) and prevToken.isOperator()) {
				return Exception(UnexpectedTokenError, lineIndex);
			}
			if (not contain({"+", "-", "!"}, nextToken.source) and nextToken.isOperator()) {
				return Exception(UnexpectedTokenError, lineIndex);
			}

			if (not contain({"this"}, prevToken.source) and prevToken.type == Keyword) {
				return Exception(UnexpectedTokenError, lineIndex);
			}
			if (not contain({"this", "not", "lambda", "new", "ref", "out"}, nextToken.source) and
			    nextToken.type == Keyword) {
				return Exception(UnexpectedTokenError, lineIndex);
			}
		}

		// SyntaxError
		if (token.type == Name) {
			if (i != 0) {
				if (contain({Name, IntNumber, CharLiteral, StringLiteral, Constant}, input[i - 1].type))
					return Exception(SyntaxError, lineIndex);
			}
			if (contain({Name, IntNumber, CharLiteral, StringLiteral, Constant}, input[i + 1].type)) {
				return Exception(SyntaxError, lineIndex);
			}
		}

		if (token.type == IntNumber) {
			if (i != 0) {
				if (contain({IntNumber, CharLiteral, StringLiteral, Constant}, prevToken.type))
					return Exception(SyntaxError, lineIndex);
			}
			if (contain({IntNumber, CharLiteral, StringLiteral, Constant}, nextToken.type)) {
				return Exception(SyntaxError, lineIndex);
			}
		}

		if (token.source == ":") {
			if (i == 0) return Exception(SyntaxError, lineIndex);
			if ((prevToken.type != Name or nextToken.type != Name) and prevToken.source != ")") {
				return Exception(SyntaxError, lineIndex);
			}
		}
	}
	if (not bracketStack.empty()) return Exception(BracketSequenceError, lineIndex);
	return Exception(Nothing);
}

Exception semanticErrorAnalysis(vector<Token> input) {
	int lineIndex = 1;
	int end = input.size();
	for (int i = 0; i < end - 1; ++i) {
		lineIndex = getLineIndex(input, i);
		Token token = input[i];
		Token prevToken = prev(input, i), nextToken = next(input, i);

		if (token.isEndOfLine()) {
			continue;
		}
		// OperatorSequenceError
		if (token.type == AssignmentOperator) {
			if (i == 0) return Exception(OperatorSequenceError, lineIndex);
			if (prevToken.type == AssignmentOperator or nextToken.type == AssignmentOperator) {
				return Exception(OperatorSequenceError, lineIndex);
			}
		}
		// OperatorPriorityError
		if (token.type == AssignmentOperator) {
			if (contain({ArithmeticOperator, AssignmentOperator, LogicalOperator, ComparisonOperator}, prevToken.type))
				return Exception(OperatorPriorityError, lineIndex);
			if (prevToken.isBinaryOperator())
				return Exception(OperatorPriorityError, lineIndex);
			if (nextToken.isOperator() and not nextToken.isUnaryOperator())
				return Exception(OperatorPriorityError, lineIndex);
		}
		//  SyntaxError
		if (token.isBinaryOperator()) {
			if (i == 0) return Exception(SyntaxError, lineIndex);
			if (nextToken.type == EOfF or prevToken.isBinaryOperator()
			    or nextToken.isBinaryOperator() or prevToken.isLeftBracket()
			    or nextToken.isRightBracket() or prevToken.isUnaryOperator()) {
				return Exception(SyntaxError, lineIndex);
			}
		}
		if (token.isUnaryOperator()) {
			if (nextToken.isEndOfExpression() or nextToken.isRightBracket() or
			    (nextToken.type == Punctuation and not nextToken.isBracket()))
				return Exception(SyntaxError, lineIndex);
		}
		// SyntaxError для Range
		if (token.type == Range) {
			if (i == 0) return Exception(SyntaxError, lineIndex);
			if (not prevToken.isRightBracket() and not prevToken.isObject())
				return Exception(SyntaxError, lineIndex);
			if (not nextToken.isLeftBracket() and not nextToken.isObject())
				return Exception(SyntaxError, lineIndex);
		}
		if (token.source == "in") {
			if (i == 0) return Exception(SyntaxError, lineIndex);
			if (not prevToken.isObject() and not prevToken.isRightBracket())
				return Exception(SyntaxError, lineIndex);
			if (not nextToken.isObject() and not prevToken.isLeftBracket())
				return Exception(SyntaxError, lineIndex);
		}
	}
	return Exception(Nothing);
}

#endif //NEKO_INTERPRETER_EXCEPTIONS_H
