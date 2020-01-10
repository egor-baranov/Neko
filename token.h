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
  constant, // служебная константа: True, False, None. Строковые и численные литералы будут отправлены в undefined
  undefined
};

struct Token {
  tokenType type;
  string source;

};

Token getToken(string input) {

}

vector<Token> tokenize(vector<string> input) {
  vector<Token> ret;
  for (string i: input)
    ret.push_back(getToken(i));
  return ret;
}

#endif //NEKO_INTERPRETER_TOKEN_H
