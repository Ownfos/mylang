# Purpose
Design and implement a transcompiler for my own language that generates C++ files

## How does this work?
```
The program will be given a list of character sequence (i.e., source files).
A lexical analyzer will read the characters and convert them into a stream of tokens.
From there, we can use the context free grammar to perform syntax analysis
and generate an abstract syntax tree (a.k.a. AST) for each source file.

The next step is traversing the AST with various visitors,
each of them representing a distinct checks such as type validation.
Passing through these semantic analysis will give information on
which module includes which, and what are the global symbols visible on each module.

A module is a logical concept which is simillar to a package or a library.
You can declare structs and functions in a module, and choose their visibility
on other modules that 'import' it by specifying 'export' prefix.

After all checks are done, a code generator will traverse the ASTs to create
a header file (xxx.h) and a source file (xxx.cpp) for each module.


             lexical        syntax      semantic     codegen
source files -------> tokens ------> AST --------> AST -------> xxx.h, xxx.cpp
```

## How does the output file layout look like?
Suppose we have three input files in this directory.  
"vector.ml" and "matrix.ml" are both implementing a module called "math".
```
vector.ml
matrix.ml
main.ml
```
If we run the program with following command, the folder will change like below.
```
> mylang ./output ./vector.ml ./matrix.ml ./main.ml
```
```
output/
├─ math.h
├─ math.cpp
├─ main.h
├─ main.cpp
vector.ml
matrix.ml
main.ml
```
Note that we only have two .h, .cpp pair instead of three.  
That is because symbols and definitions in "vector.ml" and "matrix.ml"  
got aggregated by the analyzer and are treated as if they came from a single file.

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

## How to run
```bash
# General format
.\build\src\Debug\mylang.exe [output directory] [input file 1] [input file 2] ... [input file N]

# Use sample input files (the file extension doesn't matter)
.\build\src\Debug\mylang.exe ./sample/output ./sample/circle.ml ./sample/vector.ml ./sample/main.ml
```
#### Warning: this program does not support cleaning up outputs!

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

func-decl   ::= "func" "=" "(" param-list? ")" ("->" type)? stmt

param-list  ::= param ("," param)*
param       ::= identifier ":" param-type
param-type  ::= param-usage? type
param-usage ::= "in" | "out" | "inout"

struct-decl ::= "struct" "=" "{" member-decl* "}"
member-decl ::= identifier ":" type ";"
```
```c++
// Parameter usage translation rules:
// in T    ==> const T&
// out T   ==> T&
// inout T ==> T&

// Expected code generation:
// int add(const int& a, const int& b) {
//     return (a + b);
// }
add: func = (a: i32, b: i32) -> i32 {
    return a + b;
}

// Expected code generation:
// void append(std::string& message, const std::string& prefix) {
//     (message += prefix);
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
stmt          ::= expr-stmt | var-decl-stmt | compound-stmt | if-stmt | for-stmt | while-stmt | jump-stmt
expr-stmt     ::= expr ";"
var-decl-stmt ::= var-decl ";"
compound-stmt ::= "{" stmt* "}"
if-stmt       ::= "if" "(" expr ")" compound-stmt ("else" (if-stmt | compound-stmt))?
for-stmt      ::= "for" "(" (var-decl | expr)? ";" expr? ";" expr? ")" compound-stmt
while-stmt    ::= "while" "(" expr ")" compound-stmt
jump-stmt     ::= ("return" expr? | "break" | "continue") ";"

var-decl      ::= identifier ":" type ("=" var-init)?
var-init      ::= expr | "{" var-init ("," var-init)* "}"
```
### Types
```
type            ::= base-type array-part*

base-type       ::= data-type | func-type
data-type       ::= "i32" | "f32" | "str" | "bool" | struct-type
struct-type     ::= identifier
func-type       ::= "[" "(" param-type-list? ")" ("->" type)? "]"
param-type-list ::= param-type ("," param-type)*

array-part      ::= "[" int-literal "]"
```
```c++
// A 2x2 matrix of int
i32[2][2]

// An array of custom struct type
my_struct[3]

// A function with two parameters with no return value
// 1. input paramter - array of str
// 2. output paramter - A function with two paramters and a return value
//   2-1. input parameter - int
//   2-2. input parameter - int
//   2-3. return type - array of int
[(in str[4], out [(i32, i32) -> i32[3]])]
```
### Expressions
```
expr          ::= or-expr (assign-op or-expr)*
assign-op     ::= "=" | "+=" | "-=" | "*=" | "/="

or-expr       ::= and-expr ("||" and-expr)*
and-expr      ::= compare-expr ("&&" compare-expr)*

compare-expr  ::= add-expr (compare-op add-expr)*
compare-op    ::= "==" | "!=" | ">=" | "<=" | "<" | ">"

add-expr      ::= mult-expr (("+" | "-") mult-expr)*
mult-expr     ::= prefix-expr (("*" | "/") prefix-expr)*

prefix-expr   ::= prefix-op* postfix-expr
prefix-op     ::= "!" | "+" | "-" | "++" | "--"

postfix-expr  ::= primary-expr postfix-op*
postfix-op    ::= "++" | "--" | member-access | func-call | array-index

member-access ::= "." identifier
func-call     ::= "(" arg-list? ")"
arg-list      ::= expr ("," expr)*
array-index   :: "[" expr "]"

primary-expr  ::= literal | identifier | "(" expr ")"
```

# Tree node hierarchy
- '+': abstract class
- '-': concrete class
```
+AST
 -Module
 +GlobalDecl
  -FuncDecl
  -StructDecl
 +Stmt
  -ExprStmt
  -VarDeclStmt
  -CompoundStmt
  -IfStmt
  -ForStmt
  -WhileStmt
  -JumpStmt
 +VarInit
  -VarInitExpr
  -VarInitList
 +Expr
  -Identifier
  -Literal
  -BinaryExpr
  -PrefixExpr
  -PostfixExpr
  -MemberAccessExpr
  -FunctionCallExpr
  -ArrayAccessExpr
```