#ifndef NEKO_INTERPRETER_HELPERS_H
#define NEKO_INTERPRETER_HELPERS_H

#include <bits/stdc++.h>

using namespace std;

string lStrip(string s) {
  int left = 0;
  for (int i = 0; i < s.size(); ++i) {
    if (s[i] == '\r' or s[i] == ' ' or s[i] == '\n' or s[i] == '\t') ++left;
    else break;
  }
  string ret = "";
  for (int i = left; i < s.size(); ++i)
    ret += s[i];
  return ret;
}

string rStrip(string s) {
  int right = 0;
  for (int i = s.size() - 1; i >= 0; --i) {
    if (s[i] == '\r' or s[i] == ' ' or s[i] == '\n' or s[i] == '\t') ++right;
    else break;
  }
  string ret = "";
  for (int i = 0; i < s.size() - right; ++i)
    ret += s[i];
  return ret;
}

// убрать пробелы с обеих сторон. lStrip и rStrip соответственно убирают слева и справа
string strip(string s) {
  return lStrip(rStrip(s));
}

template<typename T>
set<T> unite(const set<T> &s1, const set<T> &s2) {
  set<T> ret;
  for (T i : s1) ret.insert(i);
  for (T i: s2) ret.insert(i);
  return ret;
}

string toString(char input) {
  return string(1, input);
}

// разделение строки по char-у
vector<string> split(string s, char sep = ' ') {
  vector<string> ret;
  string tmp = "";
  for (char c : s) {
    if (c == sep) {
      if (!tmp.empty()) ret.push_back(tmp);
      tmp.clear();
      continue;
    }
    tmp += c;
  }
  if (!tmp.empty()) ret.push_back(tmp);
  return ret;
}

// разбиение выражения на токены
// = split по set-у с дополнительным разбиением на пробелы
vector<string> separate(string s, set<string> sep) {
  vector<string> ret;
  string tmp = "";
  for (char c: s) {
    if (sep.find(toString(c)) != sep.end()) {
      if (not tmp.empty()) {
        for (string i : split(strip(tmp), ' '))
          ret.push_back(i);
      }
      ret.push_back(toString(c));
      tmp = "";
    } else tmp += toString(c);
  }
  if (not tmp.empty()) {
    for (string i : split(strip(tmp), ' '))
      ret.push_back(i);
  }
  return ret;
}

string join(vector<string> v, string sep = " ") {
  string ret;
  for (int i = 0; i < v.size() - 1; ++i)
    ret += v[i] + sep;
  return ret + v.back();
}

string join(vector<string> v, char sep = ' ') {
  return join(v, string(1, sep));
}

bool isNumber(string s) {
  if (s[0] > '9' or s[0] < '0') return false;
  int dots = 0;
  for (int i = 1; i < s.size() - 1; ++i) {
    if (s[i] == '.') {
      ++dots;
      continue;
    }
    if (s[0] > '9' or s[0] < '0') return false;
  }
  if (s.back() > '9' or s.back() < '0') return false;
  return dots <= 1;
}

#endif //NEKO_INTERPRETER_HELPERS_H
