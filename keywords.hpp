#ifndef NEKO_INTERPRETER_KEYWORDS_HPP
#define NEKO_INTERPRETER_KEYWORDS_HPP

#include <bits/stdc++.h>

using namespace std;

enum Namespace {
  Global,
  InsideOfFunction,
  Local
};

set<string> BuiltInTypes{
	"int8", "int16", "int32", "int64",
	"uint8", "uint16", "uint32", "uint64",
	"char8", "char16", "char32", "uchar8", "uchar16", "uchar32",
	"float", "lfloat",
	"nullType", "string", "int", "char", "object"
};

set<string> BuiltInClasses{
	"Object",
	"Bool",
	"String", "Char",
	"Number", "Int", "Float", "Complex", "MathNum", "BigInt", "BigFloat", "BigComplex",
	"Container", "Array", "List", "Set", "Map"
};

set<string> Keywords{
	"var", "val", "in",
	"for", "while",
	"if", "else", "elif",
	"get", "set", "field", "value",
	"fun", "lambda", "return", "yield", "break", "continue",
	"class", "this", "as", "is", "new", "delete",
	"public", "private", "protected",
	"static", "sealed", "abstract",
	"from", "import",
	"thread", "ref",
	"until", "downTo"
};

set<string> Constants{
	"true", "false", "null"
};

set<string> ObjectConstants{
	"None",
	"True", "False",
	"Object"
};

set<string> Operators{
	"+", "-", "||", "*", "/", "**",
	"&", "|",
	"&&", "%", "=", "<", ">",
	"==", "!=", "<=", ">=",
	"++", "--",
	"<<", ">>", "!", "=", "<<<", ">>>",
	"^", "~",
	"+=", "-=", "*=", "**=", "^=", "&=", "|=", "/=", "%=",
	"and", "or", "not", "xor"
};

set<string> Punctuations{
	"{", "}",
	"[", "]",
	"(", ")",
	",", ";", ".", ":",
	"\"", "\'"
};

#endif //NEKO_INTERPRETER_KEYWORDS_HPP
