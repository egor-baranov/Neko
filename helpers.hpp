#ifndef NEKO_INTERPRETER_HELPERS_HPP
#define NEKO_INTERPRETER_HELPERS_HPP

#include <bits/stdc++.h>

using namespace std;

string lStrip(string input) {
	int left = 0;
	for (int i = 0; i < input.size(); ++i) {
		if (input[i] == '\r' or input[i] == ' ' or input[i] == '\n' or input[i] == '\t')
			++left;
		else break;
	}
	string ret = "";
	for (int i = left; i < input.size(); ++i)
		ret += input[i];
	return ret;
}

string rStrip(string input) {
	int right = 0;
	for (int i = input.size() - 1; i >= 0; --i) {
		if (input[i] == '\r' or input[i] == ' ' or input[i] == '\n' or input[i] == '\t')
			++right;
		else break;
	}
	string ret = "";
	for (int i = 0; i < input.size() - right; ++i)
		ret += input[i];
	return ret;
}

// убрать пробелы с обеих сторон. lStrip и rStrip соответственно убирают слева и справа
string strip(string input) {
	return lStrip(rStrip(input));
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
vector<string> split(string input, set<char> sep) {
	vector<string> ret;
	string tmp = "";
	for (char c : input) {
		if (sep.find(c) != sep.end()) {
			if (not tmp.empty()) ret.push_back(tmp);
			tmp.clear();
			continue;
		}
		tmp += c;
	}
	if (not tmp.empty()) ret.push_back(tmp);
	return ret;
}

// split, где сепараторы из сета sep добавляются в ret + удаление {' ', '\m', '\r'}
vector<string> separate(string input, set<string> sep) {
	vector<string> ret;
	string tmp = "";
	bool isChar = false, isString = false;
	for (char c: input) {
		// обработка строковых и символьных литералов
		if (c == '\n' or c == '\r') {
			isString = false;
			isChar = false;
			if (not tmp.empty())
				for (string i : split(strip(tmp), {' ', '\n', '\r'})) {
					ret.push_back(i);
				}
			tmp.clear();
		}
		if (c == '\'' and not isString) {
			if (isChar) {
				tmp += c;
				ret.push_back(tmp);
				tmp.clear();
			} else {
				if (not tmp.empty())
					for (string i : split(strip(tmp), {' ', '\n', '\r'})) {
						ret.push_back(i);
					}
				tmp.clear();
				tmp += c;
			}
			isChar = not isChar;
			continue;
		}
		if (c == '\"' and not isChar) {
			if (isString) {
				tmp += c;
				ret.push_back(tmp);
				tmp.clear();
			} else {
				if (not tmp.empty())
					for (string i : split(strip(tmp), {' ', '\n', '\r'})) {
						ret.push_back(i);
					}
				tmp.clear();
				tmp += c;
			}
			isString = not isString;
			continue;
		}
		if (isString or isChar) {
			tmp += c;
			continue;
		}
		// остальные случаи
		if (sep.find(toString(c)) != sep.end()) {
			if (not tmp.empty())
				for (string i : split(strip(tmp), {' ', '\n', '\r'})) {
					ret.push_back(i);
				}
			ret.push_back(toString(c));
			tmp.clear();
		} else tmp += toString(c);
	}
	if (not tmp.empty())
		for (string i : split(strip(tmp), {' ', '\n', '\r'})) {
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

int count(string input, char c) {
	int ret = 0;
	for (char i: input)
		if (i == c) ++ret;
	return ret;
}

bool isNumber(string input) {
	if (input[0] > '9' or input[0] < '0')
		return false;
	int dots = 0;
	for (int i = 1; i < input.size() - 1; ++i) {
		if (input[i] == '.') {
			++dots;
			continue;
		}
		if (input[i] > '9' or input[i] < '0')
			return false;
	}
	if (input.back() > '9' or input.back() < '0')
		return false;
	return dots <= 1;
}

bool isLetter(char c) {
	return (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z');
}

bool isDigit(char c) {
	return c >= '0' and c <= '9';
}

bool isDigitOrLetter(char c) {
	return isDigit(c) or isLetter(c);
}

bool isCorrectName(string input) {
	if (input.empty()) return false;
	if (not isLetter(input[0]) and input[0] != '_')
		return false;
	for (int c: input)
		if (!isDigit(c) and !isLetter(c) and c != '_')
			return false;
	return true;
}

template<typename T>
bool contain(vector<T> v, T elem) {
	for (auto i: v)
		if (i == elem) return true;
	return false;
}

template<typename T>
bool containAny(vector<T> v, vector<T> vE) {
	for (auto i : vE)
		if ((contain(v, i))) return true;
	return false;
}

template<typename T>
bool containAll(vector<T> v, vector<T> vE) {
	bool ret = true;
	for (auto i : vE)
		ret &= contain(v, i);
	return ret;
}

template<typename T>
bool min(T a, T b) {
	return (a < b ? a : b);
}

template<typename T>
bool min(T a, T b, T c) {
	return min(a, min(b, c));
}

template<typename T>
bool max(T a, T b) {
	return (a > b ? a : b);
}

template<typename T>
bool max(T a, T b, T c) {
	return max(a, max(b, c));
}

string sliceString(string s, int left, int right) {
	string ret;
	for (int i = left; i <= right; ++i) {
		ret += s[i];
	}
	return ret;
}

struct Interval {
  int left, right;

  Interval(int l, int r) : left(l), right(r) {}
};

#endif //NEKO_INTERPRETER_HELPERS_HPP
