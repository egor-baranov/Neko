#ifndef NEKO_INTERPRETER_TOKEN_H
#define NEKO_INTERPRETER_TOKEN_H

#include <bits/stdc++.h>
#include "keywords.h"
#include "helpers.h"

using namespace std;

enum tokenType {
  EOfF, EOL,
  Name, // пользовательские имена
  Keyword, // служебное ключевое слово: if, for, and, or
  Constant, // служебная константа: True, False, None. Строковые и численные литералы будут отправлены в undefined,
  Punctuation,
  Number,
  Operation,
  None
};

string toString(tokenType t) {
  if (t == EOfF) return "EOfF";
  if (t == EOL) return "EOL";
  if (t == Name) return "Name";
  if (t == Keyword) return "Keyword";
  if (t == Constant) return "Constant";
  if (t == Punctuation) return "Punctuation";
  if (t == Number) return "Number";
  if (t == Operation) return "Operation";
  return "Name";
}

struct Token {
  tokenType type;
  string source;

  Token(tokenType t, string s) : type(t), source(s) {};
};

const Token endOfLine(EOL, "\n");

Token getToken(string input) {
  input = strip(input);
  if (Punctuations.find(input) != Punctuations.end())
    return Token(Punctuation, input);
  if (Constants.find(input) != Constants.end())
    return Token(Constant, input);
  if (Keywords.find(input) != Keywords.end())
    return Token(Keyword, input);
  if (Operators.find(input) != Operators.end())
    return Token(Operation, input);
  if (isNumber(input))
    return Token(Number, input);
  return Token(Name, input);
}

vector<Token> tokenize(vector<string> input) {
  vector<Token> ret;
  for (string i: input)
    ret.push_back(getToken(i));
  // добавление конца строки, Token(EOL, "\n")
  ret.push_back(endOfLine);
  return ret;
}

vector<Token> tokenize(string input){
  return tokenize(separate(input, unite(Punctuations, Operators)));
}

#endif //NEKO_INTERPRETER_TOKEN_H
