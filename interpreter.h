#ifndef NEKO_INTERPRETER_INTERPRETER_H
#define NEKO_INTERPRETER_INTERPRETER_H

#include <bits/stdc++.h>
#include "token.h"

using namespace std;

vector<Token> tokenizeFile(string url) {
  vector<Token> ret;
  ifstream file(url);
  ofstream out("uncommented.txt");
  string tmp, use;
  bool commented = false;
  // считывание из файла
  while (getline(file, tmp)) {
    // обработка комментариев
    use = "";
    int end = tmp.size() - 1;
    for (int i = 0; i < tmp.size(); ++i) {
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
    if (use.size() > 0) out << use;

  }
  return ret;
}

#endif //NEKO_INTERPRETER_INTERPRETER_H
