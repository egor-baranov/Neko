#ifndef NEKO_INTERPRETER_NEKOLIB_HPP
#define NEKO_INTERPRETER_NEKOLIB_HPP

#include "expressions.hpp"

FunctionReturned print(vector<Item> &input) {
	int end = input.size() - 1;
	for (int i = 0; i < end; ++i) {
		printf("%s", (input[i].toString() + " ").c_str());
	}
	if (not input.empty()) {
		printf("%s", input.back().toString().c_str());
	}
	return VoidResult;
}

FunctionReturned println(vector<Item> &input) {
	print(input);
	printf("%s", "\n");
	return VoidResult;
}

FunctionReturned readLine(vector<Item> &input) {
	if (input.size() > 0) {
		return Exception(FunctionArgumentExcess);
	}
	string ret;
	getline(cin, ret);
	return Item(getToken("\"" + ret + "\""));
}

FunctionReturned readInt(vector<Item> &input) {
	if (input.size() > 0) {
		return Exception(FunctionArgumentExcess);
	}
	string s;
	cin >> s;
	Item ret = Item(getToken(s), IntType);
	if (ret.type != IntType) {
		return Exception(TypeError);
	}
	return ret;
}

FunctionReturned readFloat(vector<Item> &input) {
	if (input.size() > 0) {
		return Exception(FunctionArgumentExcess);
	}
	string s;
	cin >> s;
	Item ret = Item(getToken(s), FloatType);
	if (ret.type != FloatType) {
		return Exception(TypeError);
	}
	return ret;
}

FunctionReturned readString(vector<Item> &input) {
	if (input.size() > 0) {
		return Exception(FunctionArgumentExcess);
	}
	string s;
	cin >> s;
	Item ret = Item(getToken("\"" + s + "\""), StringType);
	if (ret.type != StringType) {
		return Exception(TypeError);
	}
	return ret;
}

FunctionReturned readChar(vector<Item> &input) {
	if (input.size() > 0) {
		return Exception(FunctionArgumentExcess);
	}
	string s;
	cin >> s;
	if (s.size() != 1) {
		return Exception(CharFormatError);
	}
	Item ret = Item(getToken("\'" + s + "\'"), CharType);
	if (ret.type != CharType) {
		return Exception(TypeError);
	}
	return ret;
}

FunctionReturned readBool(vector<Item> &input) {
	if (input.size() > 0) {
		return Exception(FunctionArgumentExcess);
	}
	string s;
	cin >> s;
	Item ret = Item(getToken(s), BoolType);
	if (ret.type != BoolType) {
		return Exception(TypeError);
	}
	return ret;
}

FunctionReturned callBuiltInFunction(string functionName, vector<Item> &input) {
	if (functionName == "print") {
		return print(input);
	}
	if (functionName == "println") {
		return println(input);
	}
	if (functionName == "readLine") {
		return readLine(input);
	}
	if (functionName == "readInt") {
		return readInt(input);
	}
	if (functionName == "readFloat") {
		return readFloat(input);
	}
	if (functionName == "readString") {
		return readString(input);
	}
	if (functionName == "readChar") {
		return readChar(input);
	}
	if (functionName == "readBool") {
		return readBool(input);
	}
}

#endif //NEKO_INTERPRETER_NEKOLIB_HPP
