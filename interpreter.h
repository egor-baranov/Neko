#ifndef NEKO_INTERPRETER_INTERPRETER_H
#define NEKO_INTERPRETER_INTERPRETER_H

#include <bits/stdc++.h>
#include "token.h"
#include "exceptions.h"

using namespace std;

// синтаксический анализ
vector<Token> parse(vector<Token> input) {
  vector<Token> output;
  int end = input.size();
  for (int i = 0; i < end - 1; ++i) {
    Token token = input[i];
    if (token.type == IntNumber and i < end - 2)
      if (input[i + 1].source == "." and input[i + 2].type == IntNumber) {
        output.push_back(Token(FloatNumber, token.source + "." + input[i + 2].source));
        i += 2;
        continue;
      }
    if (contain({"<", "=", "!"}, token.source) and input[i + 1].source == "=") {
      output.push_back(Token(ComparisonOperator, token.source + "="));
      ++i;
      continue;
    }
    if (token.source == "and" or token.source == "&&") {
      output.push_back(Token(LogicalOperator, "&&"));
      continue;
    } else if (token.source == "or" or token.source == "||") {
      output.push_back(Token(LogicalOperator, "||"));
      continue;
    } else if (token.source == "not" or token.source == "!") {
      output.push_back(Token(LogicalOperator, "!"));
      continue;
    }
    if (token.source == "." and input[i + 1].source == ".") {
      output.push_back(Token(Range, ".."));
      ++i;
      continue;
    }
    if (token.source == ">") {
      if (input[i + 1].source == ">") {
        if (input[i + 2].source == ">") {
          output.push_back(Token(BitOperator, ">>>"));
          i += 2;
        } else {
          output.push_back(Token(BitOperator, ">>"));
          ++i;
        }
      } else if (input[i + 1].source == "=") {
        output.push_back(Token(ComparisonOperator, ">="));
        ++i;
      } else output.push_back(Token(ComparisonOperator, ">"));
      continue;
    }
    if (token.source == "<") {
      if (input[i + 1].source == "<") {
        if (input[i + 2].source == "<") {
          output.push_back(Token(BitOperator, "<<<"));
          i += 2;
        } else {
          output.push_back(Token(BitOperator, "<<"));
          ++i;
        }
      } else if (input[i + 1].source == "=") {
        output.push_back(Token(ComparisonOperator, "<="));
        ++i;
      } else output.push_back(Token(ComparisonOperator, "<"));
      continue;
    }
    if (contain({"+", "-", "*", "**", "^", "&", "|", "/", "%"}, token.source) and input[i + 1].source == "=") {
      output.push_back(Token(AssignmentOperator, token.source + "="));
      ++i;
      continue;
    }
    if (token.source == "=") {
      output.push_back(Token(AssignmentOperator, "="));
      continue;
    }
    if (i > 0) {
      if ((input[i - 1].type != Punctuation or input[i - 1].isRightBracket()) and token.source == "+" and
          input[i + 1].source == "+" and
          input[i + 2].type != Name and not input[i + 2].isLeftBracket()) {
        output.push_back(Token(PostfixOperation, "++"));
        ++i;
        continue;
      }
      if ((input[i - 1].type != Punctuation or input[i - 1].isRightBracket()) and token.source == "-" and
          input[i + 1].source == "-" and
          input[i + 2].type != Name and not input[i + 2].isLeftBracket()) {
        output.push_back(Token(PostfixOperation, "--"));
        ++i;
        continue;
      }
      if (input[i - 1].type != Punctuation and token.source == "*" and input[i + 1].source == "*") {
        output.push_back(Token(ArithmeticOperator, "**"));
        ++i;
        continue;
      }
    }
    output.push_back(token);
  }
  output.push_back(input[end - 1]);
  return output;
}

// выполнение программы
void run(vector<Token> input) {
  // cout << format(input) << endl;
}

// лексический анализ
void open(string url) {
  ifstream file(url);
  string tmp, use;
  vector<Token> res;
  bool commented = false;
  int lineIndex = 0;
  // считывание из файла
  while (getline(file, tmp)) {
    ++lineIndex;
    // обработка комментариев
    use = "";
    int end = tmp.size() - 1;
    for (int i = 0; i < tmp.size(); ++i) {
      if (tmp[i] == '\n' or tmp[i] == '\r' or tmp[i] == '\t') {
        continue;
      }
      // отлов всевозможных ошибок с точкой, ибо пробелы можно отловить только здесь
      if (tmp[i] == '.') {
        if (i == 0 or i == end) {
          throwException(Exception(UnexpectedDotError, lineIndex));
          return;
        }
        if ((tmp[i - 1] == '.' xor tmp[i + 1] == '.') and
            not containAny({'\n', '\r', '\t'}, (vector<char>) {tmp[i - 1], tmp[i + 1]})) {
          use += tmp[i];
          continue;
        }
        if (containAny({'\n', '\r', '\t'}, (vector<char>) {tmp[i - 1], tmp[i + 1]}) or
            (tmp[i - 1] == '.' and tmp[i + 1] == '.') or (tmp[i - 1] == ' ' and tmp[i + 1] == ' ')) {
          throwException(Exception(UnexpectedDotError, lineIndex));
          return;
        }
      }
      if (i != end) {
        if (tmp[i] == '/' and not commented) {
          if (tmp[i + 1] == '/') break;
          if (tmp[i + 1] == '*') {
            commented = true;
            ++i;
            continue;
          }
        }
        if (tmp[i] == '*' and commented and tmp[i + 1] == '/') {
          commented = false;
          ++i;
          continue;
        }
      }
      if (not commented) use += tmp[i];
    }
    for (auto i : (tokenize(use))) {
      res.push_back(i);
    }
  }
  if (commented) {
    throwException(Exception(UnterminatedComment, lineIndex));
    return;
  }
  res.push_back(endOfFile);
  Exception syntaxException = syntaxErrorAnalysis(res);
  if (syntaxException.type != Nothing) {
    throwException(syntaxException);
    return;
  }
  res = parse(res);
  Exception semanticException = semanticErrorAnalysis(res);
  if (semanticException.type != Nothing) {
    throwException(semanticException);
    return;
  }
  cout << format(res, true) << endl;
  cout << format(res, false) << endl;
  // run(res);
}

#endif //NEKO_INTERPRETER_INTERPRETER_H
