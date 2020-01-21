#ifndef NEKO_INTERPRETER_INTERPRETER_H
#define NEKO_INTERPRETER_INTERPRETER_H

#include <bits/stdc++.h>
#include "token.h"

using namespace std;

string format(string input) {
  vector<Token> t = tokenize(separate(input, unite(Punctuations, Operators)));
  vector<string> v;
  for (Token i : t) v.push_back(toString(i.type));
  return "[" + join(v, ", ") + "]";
}

vector<Token> tokenizeFile(string inUrl, string outUrl) {
  vector<Token> ret;
  ifstream file(inUrl);
  ofstream out("outUrl");
  string tmp, use;
  bool commented = false;
  // считывание из файла
  while (getline(file, tmp)) {
    // обработка комментариев
    use = "";
    int end = tmp.size() - 1;
    for (int i = 0; i < tmp.size(); ++i) {
      if (tmp[i] == '\n' or tmp[i] == '\r' or tmp[i] == '\t')
        continue;
      if (i != end) {
        if (tmp[i] == '/' and not commented) {
          if (tmp[i + 1] == '/')
            break;
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
      if (not commented)
        use += tmp[i];
    }
    if (not use.empty())
      out << format(use) << endl;
  }
  return ret;
}

// синтаксический анализ + выполнение программы
void run(vector<Token> input) {

}

// лексический анализ
void open(string url) {
  ifstream file(url);
  string tmp, use;
  bool commented = false;
  // считывание из файла
  while (getline(file, tmp)) {
    // обработка комментариев
    use = "";
    int end = tmp.size() - 1;
    for (int i = 0; i < tmp.size(); ++i) {
      if (tmp[i] == '\n' or tmp[i] == '\r' or tmp[i] == '\t')
        continue;
      if (i != end) {
        if (tmp[i] == '/' and not commented) {
          if (tmp[i + 1] == '/')
            break;
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
    if (not use.empty()) run(tokenize(use));
  }
}

#endif //NEKO_INTERPRETER_INTERPRETER_H
