#ifndef NEKO_INTERPRETER_HELPERS_H
#define NEKO_INTERPRETER_HELPERS_H

#include <bits/stdc++.h>

using namespace std;

string strip(string s) {
  int left = 0, right = 0;
  for (int i = 0; i < s.size(); ++i) {
    if (i == '\r' or i == ' ' or i == '\n' or i == '\t') ++left;
    else break;
  }
  for (int i = s.size() - 1; i >= 0; ++i) {
    if (i == '\r' or i == ' ' or i == '\n' or i == '\t') ++right;
    else break;
  }
  string ret = "";
  return ret;
}

#endif //NEKO_INTERPRETER_HELPERS_H
