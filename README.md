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
### Notation rules used to write theses:
- Characters ```() | * ?``` are used for regular expression
- Strings enclosed between double quotes are terminals (e.g., ```"module"```)
- ```identifier``` and ```literal``` are terminals
- Everything other than these are nonterminals
### Module declaration and dependencies
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
### Declarations
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
### Statements
```
stmt          ::= expr ";" | compound-stmt | if-stmt | for-stmt | while-stmt | jump-stmt
compound-stmt ::= "{" stmt* "}"
if-stmt       ::= "if" "(" expr ")" compound-stmt ("else" (if-stmt | compound-stmt))?
for-stmt      ::= "for" "(" expr? ";" expr? ";" expr? ")" compound-stmt
while-stmt    ::= "while" "(" expr ")" compound-stmt
jump-stmt     ::= ("return" expr? | "break" | "continue") ";"
```
### Expressions
```
expr          ::= (identifier assign-op)* or-expr
assign-op     ::= "=" | "+=" | "-=" | "*=" | "/="

or-expr       ::= and-expr ("||" and-expr)*
and-expr      ::= compare-expr ("&&" compare-expr)*

compare-expr  ::= add-expr (compare-op add-expr)*
compare-op    ::= "==" | "!=" | ">=" | "<=" | "<" | ">"

add-expr      ::= mult-expr (("+" | "-") mult-expr)*
mult-expr     ::= prefix-expr (("*" | "/") prefix-expr)*

prefix-expr   ::= prefix-op? postfix-expr
prefix-op     ::= "!" | "+" | "-" | "++" | "--"

postfix-expr  ::= primary-expr postfix-op*
postfix-op    ::= "++" | "--" | member-access | func-call

member-access ::= "." identifier
func-call     ::= "(" arg-list? ")"
arg-list      ::= expr ("," expr)*

primary-expr  ::= literal | identifier | "(" expr ")"
```

# Tree node hierarchy
- '+': abstract class
- '-': concrete class
```
+AST
 -Program
 +Decl
  -FuncDecl
  -StructDecl
 +Stmt
  -CompoundStmt
  -IfStmt
  -ForStmt
  -WhileStmt
  -JumpStmt
 +Expr
  -ID
  -Literal
  -BinaryOp
  -UnaryOp
  -MemberAccess
  -FunctionCall
```