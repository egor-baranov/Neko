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
	Item ret = Item(getToken(s), "Int");
	if (ret.type != "Int") {
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
	Item ret = Item(getToken(s), "Float");
	if (ret.type != "Float") {
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
	Item ret = Item(getToken("\"" + s + "\""), "String");
	if (ret.type != "String") {
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
	Item ret = Item(getToken("\'" + s + "\'"), "Char");
	if (ret.type != "Char") {
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
	Item ret = Item(getToken(s), "Bool");
	if (ret.type != "Bool") {
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
	if (contain(
		{"abs", "sqr", "sqrt",
		 "sin", "cos", "tg", "tan", "ctg", "ctan",
		 "asin", "acos", "atan", "atan2",
		 "log2", "log10", "ln", "lg"}, functionName)) {
		if (input.size() == 0) {
			return Exception(FunctionArgumentLack);
		} else if (input.size() > 1) return Exception(FunctionArgumentExcess);
		if (not input[0].isNumber()) {
			return Exception(TypeError);
		}
		double x = (input[0].type == "Int" ? *static_cast<int *>(input[0].value)
		                                   : *static_cast<double *>(input[0].value));
		if (functionName == "abs") {
			x = abs(x);
			if (input[0].type == "Int") {
				return Item(static_cast<void *>(new int((int) x)), "Int");
			}
		}
		if (functionName == "sqr") {
			x = x * x;
		}
		if (functionName == "sqrt") {
			x = sqrt(x);
		}
		if (functionName == "sin") {
			x = sin(x);
		}
		if (functionName == "cos") {
			x = cos(x);
		}
		if (contain({"tg", "tan"}, functionName)) {
			if (cos(x) == 0) return Exception(MathError);
			x = tan(x);
		}
		if (contain({"ctg", "ctan"}, functionName)) {
			if (cos(x) == 0) return Exception(MathError);
			x = 1 / tan(x);
		}
		if (contain({"log", "ln"}, functionName)) {
			if (x <= 0) return Exception(MathError);
			x = log(x);
		}
		if (functionName == "log2") {
			if (x <= 0) return Exception(MathError);
			x = log2(x);
		}
		if (contain({"log10", "lg"}, functionName)) {
			if (x <= 0) return Exception(MathError);
			x = log10(x);
		}
		return Item(static_cast<void *>(new double(x)), "Float");
	}
	if (functionName == "log") {
		if (input.size() == 0) {
			return Exception(FunctionArgumentLack);
		} else if (input.size() > 2) {
			return Exception(FunctionArgumentExcess);
		}
		if (input.size() == 1) {
			if (input[0].isNotNumber()) {
				return Exception(TypeError);
			}
			double x = (input[0].type == "Int" ? *static_cast<int *>(input[0].value)
			                                   : *static_cast<double *>(input[0].value));
			if (x <= 0) {
				return Exception(MathError);
			}
			return Item(static_cast<void *>(new double(log(x))), "Float");
		} else {
			if (input[0].isNotNumber() or input[1].isNotNumber()) {
				return Exception(TypeError);
			}
			double x = (input[0].type == "Int" ? *static_cast<int *>(input[0].value)
			                                   : *static_cast<double *>(input[0].value)),
				y = (input[0].type == "Int" ? *static_cast<int *>(input[1].value)
				                            : *static_cast<double *>(input[1].value));
			if (x <= 0 or y <= 0 or x == 1) {
				return Exception(MathError);
			}
			return Item(static_cast<void *>(new double(log(y) / log(x))), "Float");
		}
	}
}

#endif //NEKO_INTERPRETER_NEKOLIB_HPP
