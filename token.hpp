#ifndef NEKO_INTERPRETER_TOKEN_HPP
#define NEKO_INTERPRETER_TOKEN_HPP

#include <bits/stdc++.h>
#include "keywords.hpp"
#include "helpers.hpp"

using namespace std;

enum TokenType {
  EOfF, EOL, EOE, // EOE - конец выражения, чтобы несколько выражений писать через одну строчку
  Name, // пользовательские имена
  Keyword, // служебное ключевое слово: if, for, and, or
  Constant, // служебная константа: True, False, None. Строковые и численные литералы будут отправлены в undefined,
  Punctuation,
  IntNumber,
  FloatNumber,
  StringLiteral,
  CharLiteral,
  Variable,
  ClassName,
  Operation,
  RangeObject, // {левая граница}..{правая граница}
  LogicalOperator, // операторы
  ArithmeticOperator,
  AssignmentOperator,
  ComparisonOperator,
  BitOperator,
  None
};

string toString(TokenType t) {
	switch (t) {
		case EOfF:
			return "EOfF";
		case EOL:
			return "EOL";
		case EOE:
			return "EOE";
		case Name:
			return "Name";
		case Keyword:
			return "Keyword";
		case Constant:
			return "Constant";
		case Punctuation:
			return "Punctuation";
		case IntNumber:
			return "IntNumber";
		case FloatNumber:
			return "FloatNumber";
		case StringLiteral:
			return "StringLiteral";
		case CharLiteral:
			return "CharLiteral";
		case Variable:
			return "Variable";
		case ClassName:
			return "ClassName";
		case Operation:
			return "Operation";
		case RangeObject:
			return "RangeObject";
		case LogicalOperator:
			return "LogicalOperator";
		case ArithmeticOperator:
			return "ArithmeticOperator";
		case AssignmentOperator:
			return "AssignmentOperator";
		case ComparisonOperator:
			return "ComparisonOperator";
		case BitOperator:
			return "BitOperator";
		case None:
			return "None";
	}
}

struct Token {
  TokenType type;
  string source;

  Token(Token const &other) : type(other.type), source(other.source) {};

  Token(TokenType t, string s) : type(t), source(s) {};

  bool isLeftBracket() {
	  return source == "(" or source == "[" or source == "{";
  }

  bool isRightBracket() {
	  return source == ")" or source == "]" or source == "}";
  }

  bool isBracket() {
	  return isLeftBracket() or isRightBracket();
  }

  bool isOperator() {
	  return contain(
		  {Operation, AssignmentOperator, ArithmeticOperator, BitOperator, LogicalOperator, ComparisonOperator}, type);
  }

  bool isKeyword() {
	  return type == Keyword;
  }

  bool isUnaryOperator() {
	  return contain({"$+", "$-", "!", "~"}, source);
  }

  bool isEndOfExpression() {
	  return contain({EOL, EOE}, type);
  }

  bool isEndOfLine() {
	  return type == EOL;
  }

  bool isBinaryOperator() {
	  return isOperator() and not isUnaryOperator();
  }

  bool isObject() const {
	  return contain({IntNumber, FloatNumber, StringLiteral, CharLiteral, Name, Constant}, type);
  }
};

bool isBuildInType(Token token) {
	if (token.type != Name) return false;
	if (BuiltInTypes.find(token.source) != BuiltInTypes.end() or
	    BuiltInClasses.find(token.source) != BuiltInClasses.end()) {
		return true;
	}
}

const Token endOfLine(EOL, "\n");
const Token endOfFile(EOfF, "\n");
const Token endOfExpression(EOE, "\n");
const Token emptyToken(None, "");

Token getToken(string input) {
	input = strip(input);
	if (input.empty()) {
		return Token(None, "");
	}
	if (input == ";") return endOfExpression;
	if (input == "and") return Token(Operation, "&&");
	if (input == "or") return Token(Operation, "||");
	if (input == "not") return Token(Operation, "!");
	if (Punctuations.find(input) != Punctuations.end()) {
		return Token(Punctuation, input);
	}
	if (ObjectConstants.find(input) != ObjectConstants.end() or Constants.find(input) != Constants.end()) {
		return Token(Constant, input);
	}
	if (Keywords.find(input) != Keywords.end()) {
		return Token(Keyword, input);
	}
	if (Operators.find(input) != Operators.end()) {
		return Token(Operation, input);
	}
	if (isNumber(input)) {
		return Token(contain(input, '.') ? FloatNumber : IntNumber, input);
	}
	if (input[0] == '\"' and input[input.size() - 1] == '\"') {
		return Token(StringLiteral, input);
	}
	if (input[0] == '\'' and input[input.size() - 1] == '\'') {
		return Token(CharLiteral, input);
	}
	return Token(Name, input);
}

vector<Token> tokenize(vector<string> input) {
	vector<Token> ret;
	for (string i: input)
		ret.push_back(getToken(i));
	// добавление конца строки, Token(EOL, "\m")
	ret.push_back(endOfLine);
	return ret;
}

vector<Token> tokenize(string input) {
	return tokenize(separate(input, unite(Punctuations, Operators)));
}

bool isBracketPair(Token left, Token right) {
	return contain({"()", "[]", "{}"}, left.source + right.source);
}

string format(vector<Token> input, bool source = false) {
	vector<string> v;
	for (Token i : input) {
		if (i.type == EOL) v.push_back("EOL");
		else if (i.type == EOE) v.push_back("EOE");
		else if (i.type == EOfF) v.push_back("EOfF");
		else v.push_back(source ? i.source : toString(i.type));
	}
	return "[" + join(v, ", ") + "]";
}

int getLineIndex(const vector<Token> &input, int index) {
	int ret = 1;
	for (int i = 0; i < index; ++i) {
		if (input[i].type == EOL) {
			++ret;
		}
	}
	if (input[index].type == EOfF) {
		--ret;
	}
	return ret;
}

int prevIndex(vector<Token> input, int index) {
	if (index <= 0) return 0;
	--index;
	while (input[index].type == EOL and index > 0) {
		--index;
	}
	return index;
}

Token prev(vector<Token> input, int index) {
	return input[prevIndex(input, index)];
}

int nextIndex(vector<Token> input, int index) {
	++index;
	while (input[index].type == EOL and index < input.size() - 1) {
		++index;
	}
	return (index < input.size() - 1 ? index : input.size() - 1);
}

Token next(vector<Token> input, int index) {
	return input[nextIndex(input, index)];
}

int getPriority(Token token) {
	string input = token.source;
	if (not token.isOperator()) {
		throw "Type Error in getPriority";
	}
	if (contain({"||"}, token.source)) {
		return 2;
	}
	if (contain({"&&"}, token.source)) {
		return 3;
	}
	if (contain({"in"}, token.source)) {
		return 4;
	}
	if (contain({"|"}, token.source)) {
		return 5;
	}
	if (contain({"^"}, token.source)) {
		return 6;
	}
	if (contain({"&"}, token.source)) {
		return 7;
	}
	if (contain({".."}, token.source)) {
		return 8;
	}
	if (contain({"==", "!="}, token.source)) {
		return 9;
	}
	if (contain({"<", "<=", ">", ">="}, token.source)) {
		return 10;
	}
	if (contain({"<<", ">>"}, token.source)) {
		return 11;
	}
	if (contain({"+", "-"}, token.source)) {
		return 12;
	}
	if (contain({"*", "/", "%", "**"}, token.source)) {
		return 13;
	}
	if (contain({"**"}, token.source)) {
		return 14;
	}
	if (contain({"!"}, token.source)) {
		return 15;
	}
	if (contain({"$-", "$+"}, token.source)) {
		return 16;
	}
	return 1;
}

#endif //NEKO_INTERPRETER_TOKEN_HPP
