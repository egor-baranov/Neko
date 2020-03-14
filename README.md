# Neko-Interpreter

Neko is a scripting programming language inspired by Kotlin and Python, which is being developed for writing fast and powerful code in a short amount of time. It includes following techniques:

* Object-oriented and functional paradigms
* Division into dynamic and static variables and functions
* Strong typing
* Powerful built-in library
* Native work with threads and requests
* Built-in long arithmetic
* Convenient and easy to understand syntax
* Interactive mode, similar in implementation to Python
* Сonvenient package manager
* Developed in parallel IDE specifically for this language and having native support for all its features, advanced code analysis and syntax highlighting

## Language key features: 
* Every high-level types, functions and built-in data structures is based on **Object** type, which provides unified access API like `toBool()`, `toString()`, `serialize()`, `iterate()` and other methods 
* Lambda functions:
```
f = lambda(x) = x * (x + 3) % 23 ** 0.5
```
* Division into low-level C++ types like int16, string, bool, etc and high-level types based on **Object** type. For example Int, Bool, Float, String
* Partially dynamically typed variables: 
```
a: Int, String, Bool = 3 
a = True // Ok
a = "Hello" // Ok
a = 3.2 // TypeError
```
* Functions with dynamic return value determination (can be used without **Any**, because it will be used as default return type):
```
fun sum(a, b): Any = a + b 
fun pow2(a) = 2 ** a
```
* Implicit (automatic) type conversions are forbidden in the language, but operations between different numerical types are allowed to simplify calculations
* Input / output using the console and file system
* Well-developed object-oriented paradigm with the possibility of multiple inheritance, creating properties, overloaded constructors, methods, fields and various access modifiers
* The possibility of calculations in complex arithmetic using **Complex** class
* Many standard functions and data structures based on C ++ implementation (**ArrayList** implements std::vector, **Set** implements std::set, **Map** implements std::map, etc). Moreover, all container types are based on type **Container**
* Support of syntax features implemented in Kotlin, Python, Go and C++, like ability to choose between **elif** and **else if** or various initialization methods, allowing the syntax of Kotlin (using **var** / **val**), Go (with **:=**), and Python (without anything)

## JetBrains Space workspace:

https://neko.jetbrains.space/

## IDE repository:

https://github.com/egor-baranov/NekoIDE
