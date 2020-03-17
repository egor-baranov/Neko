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
	"Object", "NoneType",
	"Bool",
	"String", "Char", "Function",
	"Unit",
	"Number", "Int", "Float", "Complex", "MathNum", "BigInt", "BigFloat", "BigComplex",
	"Container",
	"Array", "ArrayList",
	"List", "MutableList",
	"Set", "MutableSet",
	"Map", "MutableMap"
	"Any"
};

set<string> BuiltInFunctions{
	"print", "println",
	"readLine", "readInt", "readFloat", "readChar", "readString", "readBool",
	"abs", "sqr", "sqrt",
	"sin", "cos", "tg", "tan", "ctg", "ctan",
	"asin", "acos", "atan", "atan2",
	"log", "log2", "log10", "ln", "lg",
	"rad2Deg", "deg2Rad", "ceil", "floor",
	"min", "max", "sum", "mul",
	"arrayOf", "intArrayOf",
	"arrayListOf", "listOf",
	"setOf", "mapOf"
};

set<string> Keywords{
	"var", "val", "in",
	"for", "while",
	"if", "else", "elif",
	"get", "set", "field", "value", "readOnly", "writeOnly", "property"
	"fun", "lambda", "return", "yield", "break", "continue",
	"class", "this", "as", "is", "new", "delete",
	"public", "private", "protected",
	"static", "sealed", "abstract",
	"virtual", "override"
	"from", "import",
	"thread", "ref",
	"until", "downTo", "step",
	"method",
	"constructor",
	"interface"
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
	"+=", "-=", "*=", "**=", "^=", "&=", "|=", "/=", "%=", ":=",
	"and", "or", "not", "xor",
	"$+", "$-" // унарные + и -
};

set<string> Punctuations{
	"{", "}",
	"[", "]",
	"(", ")",
	",", ";", ".", ":",
	"\"", "\'"
};

#endif //NEKO_INTERPRETER_KEYWORDS_HPP
