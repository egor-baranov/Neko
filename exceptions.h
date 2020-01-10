#ifndef NEKO_INTERPRETER_EXCEPTIONS_H
#define NEKO_INTERPRETER_EXCEPTIONS_H

#include <bits/stdc++.h>

using namespace std;

enum exType {
  ZeroDivision
};

string errorMessage(exType type) {

}

void throwException(exType type) {
  cerr << errorMessage(type) << endl;
}


#endif //NEKO_INTERPRETER_EXCEPTIONS_H
