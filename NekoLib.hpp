#ifndef NEKO_INTERPRETER_NEKOLIB_HPP
#define NEKO_INTERPRETER_NEKOLIB_HPP

#include "expressions.hpp"

FunctionReturned print(vector<Item> &input) {
	int i = 0;
	for (Item item : input) {
		vector<Item> v = vector<Item>(1, item);
		auto result = callConstructor("String", v);
		if (result.exception.type != Nothing) {
			return result.exception;
		}
		String s = *static_cast<String *> (result.item.value);
		if (i + 1 < input.size()) {
			printf("%s", (s.value + " ").c_str());
		} else {
			printf("%s", (s.value).c_str());
		}
		++i;
	}
	return VoidResult;
}

FunctionReturned println(vector<Item> &input) {
	auto result = print(input);
	if (result.exception.type != Nothing) {
		return result.exception;
	}
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
	if (getToken(s).type != IntNumber) {
		return Exception(InputError);
	}
	return Item(getToken(s), "Int");
}

FunctionReturned readFloat(vector<Item> &input) {
	if (input.size() > 0) {
		return Exception(FunctionArgumentExcess);
	}
	string s;
	cin >> s;
	if (getToken(s).type != FloatNumber) {
		return Exception(InputError);
	}
	return Item(getToken(s), "Float");
}

FunctionReturned readString(vector<Item> &input) {
	if (input.size() > 0) {
		return Exception(FunctionArgumentExcess);
	}
	string s;
	cin >> s;
	return Item(getToken("\"" + s + "\""), "String");
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
	return Item(getToken("\'" + s + "\'"), "Char");
}

FunctionReturned readBool(vector<Item> &input) {
	if (input.size() > 0) {
		return Exception(FunctionArgumentExcess);
	}
	string s;
	cin >> s;
	Item ret = Item(getToken(toLowercase(s) == "true" ? "True" : "False"), "Bool");
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
		 "asin", "acos", "atan",
		 "log2", "log10", "ln", "lg",
		 "rad2Deg", "deg2Rad", "ceil", "floor", "pow2"
		}, functionName)) {
		if (input.size() == 0) {
			return Exception(FunctionArgumentLack);
		} else if (input.size() > 1) return Exception(FunctionArgumentExcess);
		if (not input[0].isNumber()) {
			return Exception(TypeError);
		}
		long double x = input[0].type == "Int" ? static_cast<Int *>(input[0].value)->value
		                                       : static_cast<Float *>(input[0].value)->value;
		if (contain({"abs", "sqr"}, functionName)) {
			if (functionName == "abs") {
				x = abs(x);
			}
			if (functionName == "sqr") {
				x = x * x;
			}
			if (input[0].type == "Int") {
				return Item(static_cast<void *>(new Int((long long) x)), "Int");
			}
		}
		if (functionName == "sqrt") {
			if (x < 0) return Exception(MathError);
			x = sqrt(x);
		}
		if (functionName == "sin") {
			x = sin(x);
		}
		if (functionName == "cos") {
			x = cos(x);
		}
		if (functionName == "pow2") {
			x = pow(2, x);
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
		if (functionName == "rad2Deg") {
			x = x / M_PI * 180;
		}
		if (functionName == "deg2Rad") {
			x = x * M_PI / 180;
		}
		if (functionName == "ceil") {
			x = ceil(x);
		}
		if (functionName == "floor") {
			x = floor(x);
		}
		return Item(static_cast<void *>(new Float(x)), "Float");
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
			long double x = input[0].type == "Int" ? static_cast<Int *>(input[0].value)->value
			                                       : static_cast<Float *>(input[0].value)->value;
			if (x <= 0) {
				return Exception(MathError);
			}
			return Item(static_cast<void *>(new Float(log(x))), "Float");
		} else {
			if (input[0].isNotNumber() or input[1].isNotNumber()) {
				return Exception(TypeError);
			}
			long double x = input[0].type == "Int" ? static_cast<Int *>(input[0].value)->value
			                                       : static_cast<Float *>(input[0].value)->value,
				y = input[0].type == "Int" ? static_cast<Int *>(input[1].value)->value
				                           : static_cast<Float *>(input[1].value)->value;
			if (x <= 0 or y <= 0 or x == 1) {
				return Exception(MathError);
			}
			return Item(static_cast<void *>(new Float(log(y) / log(x))), "Float");
		}
	}
	if (functionName == "atan2") {
		if (input.size() < 2) {
			return Exception(FunctionArgumentLack);
		} else if (input.size() > 2) {
			return Exception(FunctionArgumentExcess);
		}
		if (input[0].isNotNumber() or input[1].isNotNumber()) {
			return Exception(TypeError);
		}
		double x = input[0].type == "Int" ? static_cast<Int *>(input[0].value)->value
		                                  : static_cast<Float *>(input[0].value)->value,
			y = input[0].type == "Int" ? static_cast<Int *>(input[1].value)->value
			                           : static_cast<Float *>(input[1].value)->value;
		return Item(static_cast<void *>(new Float(atan2(x, y))), "Float");
	}
	if (contain({"max", "min"}, functionName)) {
		if (input.size() == 0) {
			return Exception(FunctionArgumentLack);
		}
		Item ret = input[0];
		for (int i = 1; i < input.size(); ++i) {
			auto returned = Process(ret, input[i], getToken(functionName == "max" ? ">" : "<"));
			if (returned.exception.type != Nothing) {
				return returned.exception;
			}
			if (returned.item.type != "Bool") {
				return Exception(TypeError);
			}
			if (returned.item.source == "True") {
				ret = input[i];
			}
		}
		return ret;
	}
	if (contain({"sum", "mul"}, functionName)) {
		if (input.size() == 0) {
			return Exception(FunctionArgumentLack);
		}
		Item ret = input[0];
		for (int i = 1; i < input.size(); ++i) {
			auto returned = Process(input[i], ret, getToken(functionName == "sum" ? "+" : "*"));
			if (returned.exception.type != Nothing) {
				return returned.exception;
			}
			ret = returned.item;
		}
		return ret;
	}
	if (functionName == "type") {
		if (input.size() == 0) {
			return Exception(FunctionArgumentLack);
		}
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		return Item(static_cast<void *> (new String(input[0].type)), "String");
	}
	if (functionName == "arrayOf") {
		Array array;
		for (Item item : input) {
			array.add(item);
		}
		return Item(static_cast<void *> (new Array(array)), "Array");
	}
	if (functionName == "mutableArrayOf") {
		MutableArray array;
		for (Item item : input) {
			array.add(item);
		}
		return Item(static_cast<void *> (new MutableArray(array)), "MutableArray");
	}
	if (functionName == "arrayListOf") {
		ArrayList arrayList;
		for (Item item : input) {
			arrayList.add(item);
		}
		return Item(static_cast<void *> (new ArrayList(arrayList)), "ArrayList");
	}
	if (functionName == "mutableArrayListOf") {
		MutableArrayList mutableArrayList;
		for (Item item : input) {
			mutableArrayList.add(item);
		}
		return Item(static_cast<void *> (new MutableArrayList(mutableArrayList)), "MutableArrayList");
	}
	if (functionName == "listOf") {
		List list;
		for (Item item : input) {
			list.add(item);
		}
		return Item(static_cast<void *> (new List(list)), "List");
	}
	if (functionName == "mutableListOf") {
		MutableList mutableList;
		for (Item item : input) {
			mutableList.add(item);
		}
		return Item(static_cast<void *> (new MutableList(mutableList)), "MutableList");
	}
	if (functionName == "setOf") {
		Set set;
		for (Item item : input) {
			set.add(item);
		}
		return Item(static_cast<void *>(new Set(set)), "Set");
	}
	if (functionName == "mutableSetOf") {
		MutableSet mutableSet;
		for (Item item : input) {
			mutableSet.add(item);
		}
		return Item(static_cast<void *>(new MutableSet(mutableSet)), "Set");
	}
	if (functionName == "len") {
		if (input.size() > 1) {
			return Exception(FunctionArgumentExcess);
		}
		if (input.size() == 0) {
			return Exception(FunctionArgumentLack);
		}
		Item arg = input[0];
		if (arg.type == "String") {
			Int len = static_cast<String *>(arg.value)->length();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "Array") {
			Int len = static_cast<Array *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "MutableArray") {
			Int len = static_cast<MutableArray *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "ArrayList") {
			Int len = static_cast<ArrayList *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "MutableArrayList") {
			Int len = static_cast<MutableArrayList *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "List") {
			Int len = static_cast<List *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "MutableList") {
			Int len = static_cast<MutableList *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "Set") {
			Int len = static_cast<Set *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "MutableSet") {
			Int len = static_cast<MutableSet *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "Map") {
			Int len = static_cast<Map *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		if (arg.type == "MutableMap") {
			Int len = static_cast<MutableMap *>(arg.value)->content.size();
			return Item(static_cast<void *>(new Int(len)), "Int");
		}
		return Exception(FunctionCallError);
	}

	if (functionName == "slice") {
		if (input.size() <= 1) {
			return Exception(FunctionArgumentLack);
		}
		if (input.size() > 3) {
			return Exception(FunctionArgumentExcess);
		}

		if (input[1].type != "Int" or input[2].type != "Int") {
			return Exception(TypeError);
		}

		long long left = static_cast<Int *>(input[1].value)->value, right = static_cast<Int *>(input[2].value)->value;

		if (input[0].type == "String") {
			string s = static_cast<String *>(input[0].value)->value;
			if (left < 0 or right >= s.size()) {
				return Exception(IndexError);
			}
			string ret;
			for (int i = left; i <= right; ++i) {
				ret += s[i];
			}
			return Item(static_cast<void *>(new String(ret)), "String");
		}
		if (input[0].type == "Array") {
			vector<Item> v = static_cast<Array *>(input[0].value)->content;
			if (left < 0 or right >= v.size()) {
				return Exception(IndexError);
			}
			vector<Item> ret;
			for (int i = left; i <= right; ++i) {
				ret.push_back(v[i]);
			}
			return Item(static_cast<void *>(new Array(ret)), "Array");
		}
		if (input[0].type == "MutableArray") {
			vector<Item> v = static_cast<MutableArray *>(input[0].value)->content;
			if (left < 0 or right >= v.size()) {
				return Exception(IndexError);
			}
			vector<Item> ret;
			for (int i = left; i <= right; ++i) {
				ret.push_back(v[i]);
			}
			return Item(static_cast<void *>(new MutableArray(ret)), "MutableArray");
		}
		return Exception(FunctionCallError);
	}

	if (functionName == "getAt") {
		if (input.size() <= 1) {
			return Exception(FunctionArgumentLack);
		}
		if (input.size() > 2) {
			return Exception(FunctionArgumentExcess);
		}

		if (input[1].type != "Int") {
			return Exception(TypeError);
		}

		long long index = static_cast<Int *>(input[1].value)->value;

		if (input[0].type == "String") {
			string s = static_cast<String *>(input[0].value)->value;
			if (index < 0 or index >= s.size()) {
				return Exception(IndexError);
			}
			return Item(static_cast<void *>(new Char(s[index])), "Char");
		}
		if (input[0].type == "Array") {
			vector<Item> v = static_cast<Array *>(input[0].value)->content;
			if (index < 0 or index >= v.size()) {
				return Exception(IndexError);
			}
			return v[index];
		}
		if (input[0].type == "MutableArray") {
			vector<Item> v = static_cast<Array *>(input[0].value)->content;
			if (index < 0 or index >= v.size()) {
				return Exception(IndexError);
			}
			return v[index];
		}
		return Exception(FunctionCallError);
	}
	// TODO: доделать все ниже
	if (functionName == "setAt") {
		if (input.size() <= 2) {
			return Exception(FunctionArgumentLack);
		}
		if (input.size() > 3) {
			return Exception(FunctionArgumentExcess);
		}

		if (input[1].type != "Int") {
			return Exception(TypeError);
		}

		long long index = static_cast<Int *>(input[1].value)->value;
		Item value = input[2];

		if (input[0].type == "String") {
			return Exception(ImmutabilityError);
		}
		if (input[0].type == "Array") {
			return Exception(ImmutabilityError);
		}
		if (input[0].type == "MutableArray") {
			vector<Item> v = static_cast<MutableArray *>(input[0].value)->content;
			if (index < 0 or index >= v.size()) {
				return Exception(IndexError);
			}
			v[index] = value;
			return Item(static_cast<void *>(new MutableArray(v)), "MutableArray");
		}
		return Exception(FunctionCallError);
	}

	return Exception(UndefinedNameUsage);
}

#endif //NEKO_INTERPRETER_NEKOLIB_HPP
