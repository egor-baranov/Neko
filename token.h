#ifndef NEKO_INTERPRETER_TOKEN_H
#define NEKO_INTERPRETER_TOKEN_H

#include <bits/stdc++.h>
#include "keywords.h"
#include "helpers.h"

using namespace std;

enum tokenType {
  EOfF, EOL,
  name, // пользовательские имена
  keyword, // служебное ключевое слово: if, for, and, or
  constant, // служебная константа: True, False, None. Строковые и численные литералы будут отправлены в undefined,
  punctuation,
  number,
  operation
};

string toString(tokenType t) {
  if (t == EOfF) return "EOfF";
  if (t == EOL) return "EOL";
  if (t == name) return "name";
  if (t == keyword) return "keyword";
  if (t == constant) return "constant";
  if (t == punctuation) return "punctuation";
  if (t == number) return "number";
  if (t == operation) return "operation";
  return "name";
}

struct Token {
  tokenType type;
  string source;

  Token(tokenType t, string s) : type(t), source(s) {};
};

Token getToken(string input) {
  input = strip(input);
  if (Punctuation.find(input) != Punctuation.end())
    return Token(punctuation, input);
  if (Constants.find(input) != Constants.end())
    return Token(constant, input);
  if (Keywords.find(input) != Keywords.end())
    return Token(keyword, input);
  if (Operators.find(input) != Operators.end())
    return Token(operation, input);
  if (isNumber(input))
    return Token(number, input);
  return Token(name, input);
}

vector<Token> tokenize(vector<string> input) {
  vector<Token> ret;
  for (string i: input)
    ret.push_back(getToken(i));
  return ret;
}

#endif //NEKO_INTERPRETER_TOKEN_H
