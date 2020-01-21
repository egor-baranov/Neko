#ifndef NEKO_INTERPRETER_KEYWORDS_H
#define NEKO_INTERPRETER_KEYWORDS_H

#include <bits/stdc++.h>

using namespace std;

set<string> builtInTypes{
  "int8", "int16", "int32", "int64",
  "uint8", "uint16", "uint32", "uint64",
  "char8", "char16", "char32", "uchar8", "uchar16", "uchar32",
  "float", "lfloat"
};

set<string> Keywords{
  "var", "val",
  "and", "or", "not", "in",
  "for", "while",
  "get", "set",
  "fun", "lambda", "return", "break", "continue",
  "this",
  "class", "as", "is", "new", "delete",
  "public", "private", "protected",
  "static", "sealed", "abstract"
};

set<string> Constants{
  "None",
  "True", "False"
};

set<string> Operators{
  "+", "-", "||", "*", "/",
  "&&", "%", "=", "<", ">",
  "==", "!=", "<=", ">=",
  "++", "--",
  "<<", ">>", "!", "="
};

set<string> Punctuations{
  "{", "}",
  "[", "]",
  "(", ")",
  ",", ";", "."
};

#endif //NEKO_INTERPRETER_KEYWORDS_H
