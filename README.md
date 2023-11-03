# Purpose
Design and implement a transcompiler for my own language that generates C++ files

## How to build
### Step 1) clone vcpkg
```
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.bat
```
### Step 2) cmake
```
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build
```

## How to run tests
```
cd build; ctest; cd ..
```

# Syntax
## Module declaration and dependencies
```
program       ::= module-decl module-import* global-decl*
module-decl   ::= "module" identifier ";"
module-import ::= "import" "export"? identifier ";"
```
```c++
// This file implements symbols for module "math".
module math;

// Public exposure of symbols in module "common".
// Dependency on "common" will be propagated
// to other files that import "math".
import export common;

// Private exposure of symbols in module "logger".
import logger;

/* ... */
```
## Declarations
```
global-decl ::= "export"? identifier ":" (func-decl | struct-decl)

func-decl   ::= "func" "=" "(" param-list? ")" ("->" type)? compound-stmt

param-list  ::= param ("," param)*
param       ::= identifier ":" param-usage? type
param-usage ::= "in" | "out" | "inout"

struct-decl ::= "struct" "=" "{" member-decl* "}"
member-decl ::= identifier ":" type ";"
```
```c++
// Expected code generation:
// int add(int a, int b) {
//     return a + b;
// }
add: func = (a: i32, b: i32) -> i32 {
    return a + b;
}

// Expected code generation:
// void append(std::string& message, std::string prefix) {
//     message += prefix;
// }
append: func = (message: out str, prefix: str) {
    message += prefix;
}

// Expected code generation:
// struct Student {
//     std::string name;
//     int age;
// }
Student: struct = {
    name: str;
    age: i32;
};
```
## Statements
```
compound-stmt ::= "{" stmt* "}"
stmt ::= 
```
## Expressions
```
expr ::=
```
- Characters ```() | * ?``` are for regular expression
- Terminals are enclosed in double quotes (e.g., ```"module"```)
- Everything other than these are nonterminals