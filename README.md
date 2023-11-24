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

## How to run (temporary) intermediate parser
```
.\build\src\Debug\mylang.exe "your source code"
```
#### Note: the main executable currently prints the AST after syntax analysis

# Pending Tasks...
- allowed operations between types
- implicit conversion?
- type analysis idea
```
type(expr(...)) => function's return type
type(expr.id) => member variable type
type(expr[int literal]) => remove leftmost array dimension
type(str + str) => str
type(i32 + i32) => i32
type(f32 + i32) => f32
type(a comp b) => bool

++, -- only allowed on i32 type

parameter requirement:
- in : rvalue or lvalue
- out / inout : lvalue only

lvalue expressions:
- identifier
- member access (if applied on an lvalue)
- array access (if applied on an lvalue)
```

# TODO
```
1. Scan for global declarations to construct a symbol table for each module
2. Iterate over each AST
2-1. Add symbol to the symbol table whenever we see a VarDeclStmt
2-2. On symbol reference, check if the symbol exists locally.
     If so, proceed to type checking.
     Otherwise, look at the module dependency information to search
     in imported modules' symbol table.
2-3. On binary operators, check if two types match or either one is implicitly convertible to another.
2-4. On array access, check if index expr has integer type and make sure the operand is indeed an array.
2-5. On member access, look for struct declaration and search for the corresponding member variable.
2-6. On function invocation, look for function declaration and check if argument types match.
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

func-decl   ::= "func" "=" "(" param-list? ")" ("->" type)? stmt

param-list  ::= param ("," param)*
param       ::= identifier ":" param-type
param-type  ::= param-usage? type
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
expr          ::= (identifier assign-op)* or-expr
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