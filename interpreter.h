#ifndef NEKO_INTERPRETER_INTERPRETER_H
#define NEKO_INTERPRETER_INTERPRETER_H

#include <bits/stdc++.h>
#include "token.h"
#include "exceptions.h"

using namespace std;

string format(vector<Token> input) {
  vector<string> v;
  for (Token i : input) v.push_back(toString(i.type));
  return "[" + join(v, ", ") + "]";
}

Exception errorAnalysis(vector<Token> input) {
  int lineIndex = 1;
  stack<Token> bracketStack;
  bool isString = false, isChar = false;
  for (int i = 0; i < input.size(); ++i) {
    Token token = input[i];
    if (token.type == EOL) {
      if (isString or isChar) {
        return Exception(QuotesSequenceError, lineIndex);
      }
      ++lineIndex;
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
    if (isBracket(token)) {
      if (isLeftBracket(token)) {
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
    // обработка неправильных имен
    if (token.type == Name and not isCorrectName(token.source)) {
      return Exception(NameError, lineIndex);
    }
    // обработка неправильного использования оператора .
  }
  return Exception(Nothing);
}

// синтаксический анализ
vector<Token> parse(vector<Token> input) {

}

// выполнение программы
void run(vector<Token> input) {
  // cout << format(input) << endl;
}

// лексический анализ
void open(string url) {
  ifstream file(url);
  string tmp, use;
  vector<Token> res;
  bool commented = false;
  int lineIndex = 0;
  // считывание из файла
  while (getline(file, tmp)) {
    ++lineIndex;
    // обработка комментариев
    use = "";
    int end = tmp.size() - 1;
    for (int i = 0; i < tmp.size(); ++i) {
      if (tmp[i] == '\n' or tmp[i] == '\r' or tmp[i] == '\t') {
        continue;
      }
      // отлов всевозможных ошибок с точкой, ибо пробелы можно отловить только здесь
      if (tmp[i] == '.') {
        if (i == 0 or i == end) {
          throwException(Exception(DotError, lineIndex));
          return;
        }
        for (auto t: {'\n', ' ', '\r', '\t'})
          if (tmp[i - 1] == t or tmp[i + 1] == t) {
            throwException(Exception(DotError, lineIndex));
            return;
          }
      }
      if (i != end) {
        if (tmp[i] == '/' and not commented) {
          if (tmp[i + 1] == '/') break;
          if (tmp[i + 1] == '*') {
            commented = true;
            ++i;
            continue;
          }
        }
        if (tmp[i] == '*' and commented and tmp[i + 1] == '/') {
          commented = false;
          ++i;
          continue;
        }
      }
      if (not commented) use += tmp[i];
    }
    for (auto i : (tokenize(use))) {
      res.push_back(i);
    }
  }
  if (commented) {
    throwException(Exception(UnterminatedComment, lineIndex));
    return;
  }
  res.push_back(endOfFile);
  cout << format(res) << endl;
  Exception exception = errorAnalysis(res);
  if (exception.type != Nothing) {
    throwException(exception);
    return;
  }
  run(res);
}

#endif //NEKO_INTERPRETER_INTERPRETER_H
