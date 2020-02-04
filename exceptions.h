#ifndef NEKO_INTERPRETER_EXCEPTIONS_H
#define NEKO_INTERPRETER_EXCEPTIONS_H

#include <bits/stdc++.h>
#include "helpers.h"

using namespace std;

enum exType {
  AssignmentError, // попытка присвоить значение к константе, а также некорректная инициализация
  OperatorPriorityError, // a *+^ b
  OperatorSequenceError, // = += *= 3
  TooManyPointsInNumber,
  MemberError, // объект не содержит поля, метода и тд
  SyntaxError,
  UnexpectedDotError, // 234 .234 или f .do() и тд
  CharFormatError, // длина символьного литерала превосходит один символ
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
  InputError,
  MathError,
  MemoryError,
  RuntimeError,
  UnexpectedTokenError,
  Nothing
};

string toString(exType type) {
  switch (type) {
    case AssignmentError:
      return "AssignmentError";
    case OperatorPriorityError:
      return "OperatorPriorityError";
    case OperatorSequenceError:
      return "OperatorSequenceError";
    case TooManyPointsInNumber:
      return "TooManyPointsInNumber";
    case MemberError:
      return "MemberError";
    case SyntaxError:
      return "SyntaxError";
    case UnexpectedDotError:
      return "UnexpectedDotError";
    case CharFormatError:
      return "CharFormatError";
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
    case UnexpectedTokenError:
      return "UnexpectedTokenError";
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

Exception syntaxErrorAnalysis(vector<Token> input) {
  int lineIndex = 1;
  stack<Token> bracketStack;
  bool isString = false, isChar = false;
  // длина входного потока
  int end = input.size();
  // последний символ - EOfF, поэтому читаем до предпоследнего
  for (int i = 0; i < end - 1; ++i) {
    Token token = input[i];
    if (token.isEndOfLine()) {
      if (isString or isChar) {
        return Exception(QuotesSequenceError, lineIndex);
      }
      ++lineIndex;
      continue;
    }
    // обработка строк
    if (token.source == "\"" and not isChar) {
      isString = not isString;
      continue;
    }
    if (token.source == "\'" and not isString) {
      isChar = not isChar;
      continue;
    }
    // обработка скобочных последовательностей
    if (token.isBracket()) {
      if (token.isLeftBracket()) {
        bracketStack.push(token);
      } else {
        if (bracketStack.empty()) {
          return Exception(BracketSequenceError, lineIndex);
        } else {
          Token tmp = bracketStack.top();
          bracketStack.pop();
          if (not isBracketPair(tmp, token))
            return Exception(BracketSequenceError, lineIndex);
        }
      }
      continue;
    }
    // обработка CharFormatError
    if (token.type == CharLiteral and token.source.size() != 3)
      return Exception(CharFormatError, lineIndex);
    // обработка неправильных имен
    if (token.type == Name and not isCorrectName(token.source)) {
      if (token.source[0] == '\"' or token.source[0] == '\'')
        return Exception(QuotesSequenceError, lineIndex);
      return Exception(NameError, lineIndex);
    }
    // MemberError для Int после Name
    // а также UnexpectedDotError - один из операндов точки ключевое слово или оператор и тд
    if (token.source == ".") {
      if (input[i - 1].source == "." or input[i + 1].source == ".") continue;

      if (input[i - 1].type != IntNumber and input[i + 1].type == IntNumber) {
        return Exception(MemberError, lineIndex);
      }
      if (token.source == "." and i == 0) {
        return Exception(UnexpectedDotError, lineIndex);
      }
      if (input[i - 1].type == Keyword or input[i - 1].isOperator() or input[i - 1].type == Constant or
          input[i + 1].type == Keyword or input[i + 1].isOperator() or input[i + 1].type == Constant) {
        if (input[i - 1].source != "this")
          return Exception(UnexpectedDotError, lineIndex);
      }
      if (not input[i - 1].isRightBracket() and input[i - 1].type == Punctuation) {
        return Exception(UnexpectedDotError, lineIndex);
      }
      if (not input[i + 1].isLeftBracket() and input[i + 1].type == Punctuation) {
        return Exception(UnexpectedDotError, lineIndex);
      }
    }
    // обработка неправильного использования оператора .  - TooManyPointsInNumber
    if (token.type == IntNumber and i > 1 and i < end - 2) {
      if (input[i - 2].type == IntNumber and input[i - 1].source == "." and
          input[i + 1].source == "." and input[i + 2].type == IntNumber) {
        return Exception(TooManyPointsInNumber, lineIndex);
      }
    }
    // обработка UnexpectedTokenError
    if (token.source == ",") {
      if (i == 0) return Exception(UnexpectedTokenError, lineIndex);

      if (not input[i - 1].isRightBracket() and input[i - 1].type == Punctuation) {
        return Exception(UnexpectedTokenError, lineIndex);
      }
      if (not input[i + 1].isLeftBracket() and input[i + 1].type == Punctuation) {
        return Exception(UnexpectedTokenError, lineIndex);
      }

      if (not contain({"+", "-"}, input[i - 1].source) and input[i - 1].isOperator()) {
        return Exception(UnexpectedTokenError, lineIndex);
      }
      if (not contain({"+", "-", "!"}, input[i + 1].source) and input[i + 1].isOperator()) {
        return Exception(UnexpectedTokenError, lineIndex);
      }

      if (input[i - 1].source != "this" and input[i - 1].type == Keyword) {
        return Exception(UnexpectedTokenError, lineIndex);
      }
      if (not contain({"this", "not", "lambda", "new"}, input[i + 1].source) and input[i + 1].type == Keyword) {
        return Exception(UnexpectedTokenError, lineIndex);
      }
    }

    // AssignmentError
    if (contain({"var", "val"}, token.source)) {
      if (i != 0) {
        if (not input[i - 1].isEndOfExpression())
          return Exception(AssignmentError, lineIndex);
      }
      if (input[i + 1].type != Name or input[i + 2].source != "=")
        return Exception(AssignmentError, lineIndex);
    }
    // SyntaxError
    if (token.type == Name) {
      if (i != 0)
        if (contain({Name, IntNumber, CharLiteral, StringLiteral, Constant}, input[i - 1].type))
          return Exception(SyntaxError, lineIndex);
      if (contain({Name, IntNumber, CharLiteral, StringLiteral, Constant}, input[i + 1].type))
        return Exception(SyntaxError, lineIndex);
    }
    if (token.type == IntNumber) {
      if (i != 0)
        if (contain({IntNumber, CharLiteral, StringLiteral, Constant}, input[i - 1].type))
          return Exception(SyntaxError, lineIndex);
      if (contain({IntNumber, CharLiteral, StringLiteral, Constant}, input[i + 1].type))
        return Exception(SyntaxError, lineIndex);
    }
    if (token.source == ":") {
      if (i == 0) return Exception(SyntaxError, lineIndex);
      if ((input[i - 1].type != Name and input[i - 1].source != ")") or input[i + 1].type != Name)
        return Exception(SyntaxError, lineIndex);
    }
    // TODO: доделать
//    if (token.type == Operation and token.source != "!" and token.source != "~") {
//      if (input[i + 1].type == EOL) {
//        if (i < 2) return Exception(SyntaxError, lineIndex);
//        if ((token.source == "+" or token.source == "-") and input[i - 1].type == Operation)
//          if (input[i - 2].type == Name and token.source == input[i - 1].source)
//            continue;
//        return Exception(SyntaxError, lineIndex);
//      }
//    }

    if (contain({"if", "for", "while"}, token.source) and input[i + 1].source != "(")
      return Exception(SyntaxError, lineIndex);
    if (contain({"fun", "lambda"}, token.source) and input[i + 2].source != "(")
      return Exception(SyntaxError, lineIndex);
  }
  return Exception(Nothing);
}

Exception semanticErrorAnalysis(vector<Token> input) {
  int lineIndex = 1;
  int end = input.size();
  for (int i = 0; i < end - 1; ++i) {
    Token token = input[i];
    if (token.isEndOfLine()) {
      ++lineIndex;
      continue;
    }
    // OperatorSequenceError
    if (token.type == AssignmentOperator) {
      if (i == 0) return Exception(OperatorSequenceError, lineIndex);
      if (input[i - 1].isEndOfExpression() or input[i + 1].isEndOfExpression()) {
        return Exception(OperatorSequenceError, lineIndex);
      }
      if (input[i - 1].type == AssignmentOperator or input[i + 1].type == AssignmentOperator) {
        return Exception(OperatorSequenceError, lineIndex);
      }
    }
    // OperatorPriorityError
    if (token.type == AssignmentOperator) {
      if (contain({ArithmeticOperator, AssignmentOperator, LogicalOperator, ComparisonOperator}, input[i - 1].type))
        return Exception(OperatorPriorityError, lineIndex);
      if (input[i + 1].isOperator() and not input[i + 1].isUnaryOperator())
        return Exception(OperatorPriorityError, lineIndex);
    }
    //  SyntaxError
    if (token.isBinaryOperator()) {
      if (i == 0) return Exception(SyntaxError, lineIndex);
      if (input[i - 1].isEndOfExpression() or input[i + 1].isEndOfExpression() or
          input[i - 1].isBinaryOperator() or input[i + 1].isBinaryOperator() or
          input[i - 1].isLeftBracket() or input[i + 1].isRightBracket()) {
        return Exception(SyntaxError, lineIndex);
      }
    }
    if (token.isUnaryOperator()) {
      if (input[i + 1].isEndOfExpression() or input[i + 1].isRightBracket() or
          (input[i + 1].type == Punctuation and not input[i + 1].isBracket()))
        return Exception(SyntaxError, lineIndex);
    }
  }
  return Exception(Nothing);
}

#endif //NEKO_INTERPRETER_EXCEPTIONS_H
