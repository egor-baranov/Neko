#ifndef NEKO_INTERPRETER_TOKEN_H
#define NEKO_INTERPRETER_TOKEN_H

#include <bits/stdc++.h>
#include "keywords.h"
#include "helpers.h"

using namespace std;

enum tokenType {
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
  Range, // {левая граница}..{правая граница}
  LogicalOperator, // операторы
  ArithmeticOperator,
  AssignmentOperator,
  ComparisonOperator,
  BitOperator,
  PostfixOperation, // ++ или --
  None
};

string toString(tokenType t) {
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
    case Range:
      return "Range";
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
    case PostfixOperation:
      return "PostfixOperation";
    case None:
      return "None";
  }
}

struct Token {
  tokenType type;
  string source;

  Token(tokenType t, string s) : type(t), source(s) {};

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

  bool isUnaryOperator() {
    return contain({"+", "-", "!", "~"}, source);
  }

  bool isEndOfExpression() {
    return contain({EOL, EOE}, type);
  }

  bool isEndOfLine() {
    return type == EOL;
  }

  bool isBinaryOperator() {
    return isOperator() and not isUnaryOperator() and type != PostfixOperation;
  }

  bool isObject() {
    return contain({IntNumber, FloatNumber, StringLiteral, CharLiteral, Name}, type);
  }
};

const Token endOfLine(EOL, "\n");
const Token endOfFile(EOfF, "\n");
const Token endOfExpression(EOE, "\n");

Token getToken(string input) {
  input = strip(input);
  if (input == ";") return endOfExpression;
  if (input == "and") return Token(Operation, "&&");
  if (input == "or") return Token(Operation, "||");
  if (input == "not") return Token(Operation, "!");
  if (Punctuations.find(input) != Punctuations.end())
    return Token(Punctuation, input);
  if (ObjectConstants.find(input) != ObjectConstants.end() or Constants.find(input) != Constants.end())
    return Token(Constant, input);
  if (Keywords.find(input) != Keywords.end())
    return Token(Keyword, input);
  if (Operators.find(input) != Operators.end())
    return Token(Operation, input);
  if (isNumber(input))
    return Token(IntNumber, input);
  if (input[0] == '\"' and input[input.size() - 1] == '\"')
    return Token(StringLiteral, input);
  if (input[0] == '\'' and input[input.size() - 1] == '\'')
    return Token(CharLiteral, input);
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
  return (left.source == "(" and right.source == ")") or
         (left.source == "[" and right.source == "]") or (left.source == "{" and right.source == "}");
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

int getLineIndex(vector<Token> input, int index) {
  int ret = 1;
  for (int i = 0; i < index; ++i) {
    if (input[i].type == EOL) ++ret;
  }
  return ret;
}

int prevIndex(vector<Token> input, int index) {
  if (index <= 0) return 0;
  --index;
  while (input[index].type == EOL and index > 0) --index;
  return index;
}

Token prev(vector<Token> input, int index) {
  return input[prevIndex(input, index)];
}

int nextIndex(vector<Token> input, int index) {
  ++index;
  while (input[index].type == EOL and index < input.size() - 1) ++index;
  return (index < input.size() - 1 ? index : input.size() - 1);
}

Token next(vector<Token> input, int index) {
  return input[nextIndex(input, index)];
}

#endif //NEKO_INTERPRETER_TOKEN_H
