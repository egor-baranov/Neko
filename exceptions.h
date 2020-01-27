#ifndef NEKO_INTERPRETER_EXCEPTIONS_H
#define NEKO_INTERPRETER_EXCEPTIONS_H

#include <bits/stdc++.h>

using namespace std;

enum exType {
  SyntaxError,
  DotError, // 234 .234 или f .do() и тд
  UnterminatedComment,
  ZeroDivisionError,
  NameError,
  BracketSequenceError,
  QuotesSequenceError,
  ElementSequenceError,
  FunctionCallError,
  FunctionArgumentExcess,
  FunctionArgumentLack,
  IncorrectOperationArguments,
  OperatorSequenceError,
  InputError,
  MathError,
  MemoryError,
  RuntimeError,
  Nothing
};

string toString(exType type) {
  switch (type) {
    case SyntaxError:
      return "SyntaxError";
    case DotError:
      return "DotError";
    case UnterminatedComment:
      return "UnterminatedComment";
    case ZeroDivisionError:
      return "ZeroDivisionError";
    case NameError:
      return "NameError";
    case BracketSequenceError:
      return "BracketSequenceError";
    case QuotesSequenceError:
      return "QuotesSequenceError";
    case ElementSequenceError:
      return "ElementSequenceError";
    case FunctionCallError:
      return "FunctionCallError";
    case FunctionArgumentExcess:
      return "FunctionArgumentExcess";
    case FunctionArgumentLack:
      return "FunctionArgumentLack";
    case IncorrectOperationArguments:
      return "IncorrectOperationArguments";
    case OperatorSequenceError:
      return "OperatorSequenceError";
    case InputError:
      return "InputError";
    case MathError:
      return "MathError";
    case MemoryError:
      return "MemoryError";
    case RuntimeError:
      return "RuntimeError";
    case Nothing:
      return "Nothing";
  }
}

struct Exception {
  exType type;
  int line;

  Exception(exType eT) {
    type = eT;
    line = -1;
  }

  Exception(exType eT, int l) : type(eT), line(l) {}
};

string errorMessage(Exception ex) {
  return toString(ex.type) + " in line " + to_string(ex.line);
}

void throwException(Exception ex) {
  cerr << errorMessage(ex) << endl;
}

#endif //NEKO_INTERPRETER_EXCEPTIONS_H
