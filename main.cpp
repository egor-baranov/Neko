#include <bits/stdc++.h>
#include "interpreter.h"
#include "helpers.h"

using namespace std;

int main() {
  string s;
  while (getline(cin, s)) {
    vector<Token> t = tokenize(separate(s, unite(Punctuation, Operators)));
    vector<string> v, u;
    for (Token i : t) v.push_back(i.source);
    cout << "[" + join(v, ", ") + "]" << endl;
    for (Token i : t) u.push_back(toString(i.type));
    cout << "[" + join(u, ", ") + "]" << endl;
  }
  return 0;
}
