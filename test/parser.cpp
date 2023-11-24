#include "file/DummySourceFile.h"
#include "lexer/LexicalAnalyzer.h"
#include "lexer/DummyLexicalAnalyzer.h"
#include "parser/routine/ExprParser.h"
#include "parser/routine/StmtParser.h"
#include "parser/routine/TypeParser.h"
#include "parser/routine/GlobalDeclParser.h"
#include "parser/routine/ModuleParser.h"
#include "parser/SyntaxAnalyzer.h"
#include "parser/ast/visitor/TreePrinter.h"
#include "parser/ast/visitor/GlobalSymbolScanner.h"
#include "parser/ast/visitor/TypeChecker.h"
#include "parser/SemanticError.h"
#include "parser/type/Type.h"
#include "parser/type/base/PrimitiveType.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace mylang;

Type CreateDummyIntArrayType(const std::vector<int>& array_sizes)
{
    auto base_type = std::make_shared<PrimitiveType>(Token{.type = TokenType::IntType, .lexeme="i32"});
    return Type(base_type, array_sizes);
}

TEST(Type, RemoveArrayDimension)
{
    auto type = CreateDummyIntArrayType({2, 1});

    ASSERT_EQ(type.ToString(), "i32[2][1]");

    type.RemoveLeftmostArrayDim();
    ASSERT_EQ(type.ToString(), "i32[1]");

    type.RemoveLeftmostArrayDim();
    ASSERT_EQ(type.ToString(), "i32");
}

TEST(Type, AddArrayDimension)
{
    auto type = CreateDummyIntArrayType({});

    ASSERT_EQ(type.ToString(), "i32");

    type.AddLeftmostArrayDim(1);
    ASSERT_EQ(type.ToString(), "i32[1]");

    type.AddLeftmostArrayDim(2);
    ASSERT_EQ(type.ToString(), "i32[2][1]");
}

TEST(Type, MergeArraySizeSameDimension)
{
    auto type1 = CreateDummyIntArrayType({2, 3});
    auto type2 = CreateDummyIntArrayType({4, 1});
    type1.MergeArrayDim(type2);

    ASSERT_EQ(type1.ToString(), "i32[4][3]");
}

TEST(Type, MergeArraySizeDifferentDimension)
{
    auto type1 = CreateDummyIntArrayType({2, 3});
    auto type2 = CreateDummyIntArrayType({1});

    // Mixing arrays of different dimension is not allowed!
    EXPECT_THROW(type1.MergeArrayDim(type2), std::exception);
}

void TestTypeParser(const std::vector<Token>& tokens, std::string_view expected)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(tokens);
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = TypeParser(token_stream);
    auto type = parser.Parse();

    ASSERT_EQ(type.ToString(), expected);
}

TEST(TypeParser, Primitive)
{
    auto tokens = std::vector<Token>{
        {TokenType::IntType, "i32"}
    };

    auto expected = "i32";

    TestTypeParser(tokens, expected);
}

TEST(TypeParser, SimpleArray)
{
    auto tokens = std::vector<Token>{
        {TokenType::IntType, "i32"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "100"},
        {TokenType::RightBracket, "]"}
    };

    auto expected = "i32[100]";

    TestTypeParser(tokens, expected);
}

TEST(TypeParser, MultiDimensionArray)
{
    auto tokens = std::vector<Token>{
        {TokenType::IntType, "i32"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "10"},
        {TokenType::RightBracket, "]"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "20"},
        {TokenType::RightBracket, "]"}
    };
    
    auto expected = "i32[10][20]";

    TestTypeParser(tokens, expected);
}

TEST(TypeParser, SimpleCallable)
{
    auto tokens = std::vector<Token>{
        {TokenType::LeftBracket, "["},
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"},
        {TokenType::RightBracket, "]"}
    };
    
    auto expected = "[()]";

    TestTypeParser(tokens, expected);
}

TEST(TypeParser, SimpleCallableWithReturn)
{
    auto tokens = std::vector<Token>{
        {TokenType::LeftBracket, "["},
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"},
        {TokenType::Arrow, "->"},
        {TokenType::BoolType, "bool"},
        {TokenType::RightBracket, "]"}
    };

    auto expected = "[() -> bool]";

    TestTypeParser(tokens, expected);
}

TEST(TypeParser, TwoArgCallable)
{
    auto tokens = std::vector<Token>{
        {TokenType::LeftBracket, "["},
        {TokenType::LeftParen, "("},
        {TokenType::IntType, "i32"},
        {TokenType::Comma, ","},
        {TokenType::StringType, "str"},
        {TokenType::RightParen, ")"},
        {TokenType::RightBracket, "]"}
    };

    auto expected = "[(in i32, in str)]";

    TestTypeParser(tokens, expected);
}

void TestExprParser(const std::vector<Token>& tokens, std::string_view expected)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(tokens);
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto type = parser.Parse();

    ASSERT_EQ(type->ToString(), expected);
}

TEST(ExprParser, Identifier)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "foo"}
    };

    auto expected = "foo";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, BasicOperators)
{
    auto tokens = std::vector<Token>{
        {TokenType::IntLiteral, "1"},
        {TokenType::Plus, "+"},
        {TokenType::IntLiteral, "2"},
        {TokenType::Multiply, "*"},
        {TokenType::FloatLiteral, "3.0"}
    };

    auto expected = "(1 + (2 * 3.0))";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, MemberAccess)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "something"},
        {TokenType::Period, "."},
        {TokenType::Identifier, "member"}
    };

    auto expected = "something.member";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, ConsecutiveMemberAcccess)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "something"},
        {TokenType::Period, "."},
        {TokenType::Identifier, "member1"},
        {TokenType::Period, "."},
        {TokenType::Identifier, "member2"}
    };

    auto expected = "something.member1.member2";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, FuncCallNoArg)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "foo"},
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"}
    };

    auto expected = "foo()";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, FuncCallSingleArg)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "foo"},
        {TokenType::LeftParen, "("},
        {TokenType::StringLiteral, "\"hello, world!\""},
        {TokenType::RightParen, ")"}
    };

    auto expected = "foo(\"hello, world!\")";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, FuncCallMultipleArg)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "foo"},
        {TokenType::LeftParen, "("},
        {TokenType::IntLiteral, "1"},
        {TokenType::Comma, ","},
        {TokenType::IntLiteral, "2"},
        {TokenType::Comma, ","},
        {TokenType::IntLiteral, "3"},
        {TokenType::RightParen, ")"}
    };

    auto expected = "foo(1, 2, 3)";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, ArrayAccess)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "arr"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "1"},
        {TokenType::RightBracket, "]"}
    };

    auto expected = "arr[1]";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, ConsecutiveArrayAccess)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "arr"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "1"},
        {TokenType::RightBracket, "]"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "2"},
        {TokenType::RightBracket, "]"}
    };

    auto expected = "arr[1][2]";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, ExprWithParenthesis)
{
    auto tokens = std::vector<Token>{
        {TokenType::LeftParen, "("},
        {TokenType::IntLiteral, "1"},
        {TokenType::Plus, "+"},
        {TokenType::IntLiteral, "2"},
        {TokenType::RightParen, ")"},
        {TokenType::Divide, "/"},
        {TokenType::IntLiteral, "3"}
    };

    auto expected = "((1 + 2) / 3)";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, PrefixOperators)
{
    auto tokens = std::vector<Token>{
        {TokenType::Increment, "++"},
        {TokenType::Decrement, "--"},
        {TokenType::Not, "!"},
        {TokenType::Identifier, "i"}
    };

    auto expected = "(++(--(!i)))";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, PostfixOperators)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "i"},
        {TokenType::Increment, "++"},
        {TokenType::Decrement, "--"}
    };

    auto expected = "((i++)--)";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, Assignments)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "i"},
        {TokenType::Assign, "="},
        {TokenType::Identifier, "j"},
        {TokenType::PlusAssign, "+="},
        {TokenType::IntLiteral, "1"}
    };

    auto expected = "(i = (j += 1))";

    TestExprParser(tokens, expected);
}

TEST(ExprParser, LogicalOperators)
{
    auto tokens = std::vector<Token>{
        {TokenType::BoolLiteral, "true"},
        {TokenType::And, "&&"},
        {TokenType::BoolLiteral, "false"},
        {TokenType::Or, "||"},
        {TokenType::BoolLiteral, "true"}
    };

    auto expected = "((true && false) || true)";

    TestExprParser(tokens, expected);
}

void TestStmtParser(const std::vector<Token>& tokens, std::string_view expected)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(tokens);
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, Expression)
{
    auto tokens = std::vector<Token>{
        {TokenType::BoolLiteral, "true"},
        {TokenType::Semicolon, ";"}
    };

    auto expected =
        "[ExprStmt]\n"
        "    [Literal]\n"
        "    - true\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, VariableDeclaration)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "count"},
        {TokenType::Colon, ":"},
        {TokenType::IntType, "i32"},
        {TokenType::Assign, "="},
        {TokenType::IntLiteral, "0"},
        {TokenType::Semicolon, ";"}
    };

    auto expected =
        "[VarDeclStmt]\n"
        "- name: count\n"
        "- type: i32\n"
        "    [VarInitExpr]\n"
        "        [Literal]\n"
        "        - 0\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, VariableDeclarationWithoutInit)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "count"},
        {TokenType::Colon, ":"},
        {TokenType::IntType, "i32"},
        {TokenType::Semicolon, ";"}
    };

    auto expected =
        "[VarDeclStmt]\n"
        "- name: count\n"
        "- type: i32\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, CompoundStatement)
{
    auto tokens = std::vector<Token>{
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected = "[CompoundStmt]\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, IfStatement)
{
    auto tokens = std::vector<Token>{
        {TokenType::If, "if"},
        {TokenType::LeftParen, "("},
        {TokenType::BoolLiteral, "true"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected = 
        "[IfStmt]\n"
        "    [Literal]\n"
        "    - true\n"
        "    [CompoundStmt]\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, IfElseStatement)
{
    auto tokens = std::vector<Token>{
        {TokenType::If, "if"},
        {TokenType::LeftParen, "("},
        {TokenType::BoolLiteral, "true"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"},
        {TokenType::Else, "else"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected =
        "[IfStmt]\n"
        "    [Literal]\n"
        "    - true\n"
        "    [CompoundStmt]\n"
        "    [CompoundStmt]\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, IfElseIfStatement)
{
    auto tokens = std::vector<Token>{
        {TokenType::If, "if"},
        {TokenType::LeftParen, "("},
        {TokenType::BoolLiteral, "true"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"},
        {TokenType::Else, "else"},
        {TokenType::If, "if"},
        {TokenType::LeftParen, "("},
        {TokenType::BoolLiteral, "false"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected =
        "[IfStmt]\n"
        "    [Literal]\n"
        "    - true\n"
        "    [CompoundStmt]\n"
        "    [IfStmt]\n"
        "        [Literal]\n"
        "        - false\n"
        "        [CompoundStmt]\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, WhileStatement)
{
    auto tokens = std::vector<Token>{
        {TokenType::While, "while"},
        {TokenType::LeftParen, "("},
        {TokenType::BoolLiteral, "true"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected =
        "[WhileStmt]\n"
        "    [Literal]\n"
        "    - true\n"
        "    [CompoundStmt]\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, ReturnStatement)
{
    auto tokens = std::vector<Token>{
        {TokenType::Return, "return"},
        {TokenType::Semicolon, ";"}
    };

    auto expected =
        "[JumpStmt]\n"
        "- jump type: return\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, ReturnExprStatement)
{
    auto tokens = std::vector<Token>{
        {TokenType::Return, "return"},
        {TokenType::IntLiteral, "0"},
        {TokenType::Semicolon, ";"}
    };

    auto expected =
        "[JumpStmt]\n"
        "- jump type: return\n"
        "    [Literal]\n"
        "    - 0\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, SimpleForStatement)
{
    auto tokens = std::vector<Token>{
        {TokenType::For, "for"},
        {TokenType::LeftParen, "("},
        {TokenType::Semicolon, ";"},
        {TokenType::Semicolon, ";"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected =
        "[ForStmt]\n"
        "    [CompoundStmt]\n";
    
    TestStmtParser(tokens, expected);
}

TEST(StmtParser, ForStatementWithInit)
{
    auto tokens = std::vector<Token>{
        {TokenType::For, "for"},
        {TokenType::LeftParen, "("},
        {TokenType::Identifier, "i"},
        {TokenType::Colon, ":"},
        {TokenType::IntType, "i32"},
        {TokenType::Assign, "="},
        {TokenType::IntLiteral, "0"},
        {TokenType::Semicolon, ";"},
        {TokenType::Semicolon, ";"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected =
        "[ForStmt]\n"
        "    [VarDeclStmt]\n"
        "    - name: i\n"
        "    - type: i32\n"
        "        [VarInitExpr]\n"
        "            [Literal]\n"
        "            - 0\n"
        "    [CompoundStmt]\n";
    
    TestStmtParser(tokens, expected);
}

void TestGlobalDeclParser(const std::vector<Token>& tokens, std::string_view expected)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(tokens);
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto stmt_parser = std::make_shared<StmtParser>(token_stream, expr_parser, type_parser);
    auto parser = GlobalDeclParser(token_stream, stmt_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    ASSERT_EQ(output.str(), expected);
}

TEST(GlobalDeclParser, SimpleFunction)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "foo"},
        {TokenType::Colon, ":"},
        {TokenType::Func, "func"},
        {TokenType::Assign, "="},
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected =
        "[FuncDecl]\n"
        "- name: foo\n"
        "- export: false\n"
        "- return type: void\n"
        "    [CompoundStmt]\n";
        
    TestGlobalDeclParser(tokens, expected);
}

TEST(GlobalDeclParser, TwoArgsFunction)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "foo"},
        {TokenType::Colon, ":"},
        {TokenType::Func, "func"},
        {TokenType::Assign, "="},
        {TokenType::LeftParen, "("},
        {TokenType::Identifier, "a"},
        {TokenType::Colon, ":"},
        {TokenType::IntType, "i32"},
        {TokenType::Comma, ","},
        {TokenType::Identifier, "b"},
        {TokenType::Colon, ":"},
        {TokenType::Out, "out"},
        {TokenType::IntType, "i32"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected =
        "[FuncDecl]\n"
        "- name: foo\n"
        "- export: false\n"
        "- return type: void\n"
        "    [Parameter]\n"
        "    - name: a\n"
        "    - type: in i32\n"
        "    [Parameter]\n"
        "    - name: b\n"
        "    - type: out i32\n"
        "    [CompoundStmt]\n";
        
    TestGlobalDeclParser(tokens, expected);
}

TEST(GlobalDeclParser, FunctionWithReturnType)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "foo"},
        {TokenType::Colon, ":"},
        {TokenType::Func, "func"},
        {TokenType::Assign, "="},
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"},
        {TokenType::Arrow, "->"},
        {TokenType::BoolType, "bool"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    };

    auto expected =
        "[FuncDecl]\n"
        "- name: foo\n"
        "- export: false\n"
        "- return type: bool\n"
        "    [CompoundStmt]\n";
        
    TestGlobalDeclParser(tokens, expected);
}

TEST(GlobalDeclParser, StructDecl)
{
    auto tokens = std::vector<Token>{
        {TokenType::Identifier, "vec3"},
        {TokenType::Colon, ":"},
        {TokenType::Struct, "struct"},
        {TokenType::Assign, "="},
        {TokenType::LeftBrace, "{"},
        {TokenType::Identifier, "x"},
        {TokenType::Colon, ":"},
        {TokenType::FloatType, "f32"},
        {TokenType::Semicolon, ";"},
        {TokenType::Identifier, "y"},
        {TokenType::Colon, ":"},
        {TokenType::FloatType, "f32"},
        {TokenType::Semicolon, ";"},
        {TokenType::Identifier, "z"},
        {TokenType::Colon, ":"},
        {TokenType::FloatType, "f32"},
        {TokenType::Semicolon, ";"},
        {TokenType::RightBrace, "}"}
    };

    auto expected =
        "[StructDecl]\n"
        "- name: vec3\n"
        "- export: false\n"
        "- member name: x, type: f32\n"
        "- member name: y, type: f32\n"
        "- member name: z, type: f32\n";
        
    TestGlobalDeclParser(tokens, expected);
}

void TestModuleParser(const std::vector<Token>& tokens, std::string_view expected)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(tokens);
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto stmt_parser = std::make_shared<StmtParser>(token_stream, expr_parser, type_parser);
    auto global_decl_parser = std::make_shared<GlobalDeclParser>(token_stream, stmt_parser, type_parser);
    auto parser = ModuleParser(token_stream, global_decl_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    ASSERT_EQ(output.str(), expected);
}

TEST(ModuleParser, SimpleModule)
{
    auto tokens = std::vector<Token>{
        {TokenType::Module, "module"},
        {TokenType::Identifier, "test"},
        {TokenType::Semicolon, ";"}
    };

    auto expected =
        "[Module]\n"
        "- module: test\n";
        
    TestModuleParser(tokens, expected);
}

TEST(ModuleParser, ModuleWithImports)
{
    auto tokens = std::vector<Token>{
        {TokenType::Module, "module"},
        {TokenType::Identifier, "test"},
        {TokenType::Semicolon, ";"},
        {TokenType::Import, "import"},
        {TokenType::Identifier, "math"},
        {TokenType::Semicolon, ";"},
        {TokenType::Import, "import"},
        {TokenType::Export, "export"},
        {TokenType::Identifier, "random"},
        {TokenType::Semicolon, ";"},
    };

    auto expected =
        "[Module]\n"
        "- module: test\n"
        "- imported module: math, export: false\n"
        "- imported module: random, export: true\n";
        
    TestModuleParser(tokens, expected);
}

std::shared_ptr<IAbstractSyntaxTree> GenerateAST(std::string&& source_code)
{
    auto source_file = std::make_unique<DummySourceFile>(std::move(source_code));
    auto lexer = std::make_unique<LexicalAnalyzer>(std::move(source_file));
    auto syntax_analyzer = SyntaxAnalyzer(std::move(lexer));

    return syntax_analyzer.GenerateAST();
}

TEST(GlobalSymbolScanner, SingleFile)
{
    auto environment = ProgramEnvironment();
    auto scanner = GlobalSymbolScanner(environment);

    auto ast = GenerateAST("module a; foo: func = (){}");
    ast->Accept(&scanner);

    auto symbol = environment.FindSymbol("a", "foo");
    ASSERT_FALSE(symbol.is_public);
    ASSERT_EQ(symbol.scope_level, 0);
    ASSERT_EQ(symbol.declaration->DeclType().ToString(), "[()]");
}

TEST(GlobalSymbolScanner, SingleFileMultipleDecl)
{
    auto environment = ProgramEnvironment();
    auto scanner = GlobalSymbolScanner(environment);

    auto ast = GenerateAST("module a; A: struct = {} export B: struct = {}");
    ast->Accept(&scanner);

    auto symbol_a = environment.FindSymbol("a", "A");
    ASSERT_FALSE(symbol_a.is_public);
    ASSERT_EQ(symbol_a.scope_level, 0);
    ASSERT_EQ(symbol_a.declaration->DeclType().ToString(), "A");

    auto symbol_b = environment.FindSymbol("a", "B");
    ASSERT_TRUE(symbol_b.is_public);
    ASSERT_EQ(symbol_b.scope_level, 0);
    ASSERT_EQ(symbol_b.declaration->DeclType().ToString(), "B");
}

TEST(GlobalSymbolScanner, TwoFiles)
{
    auto environment = ProgramEnvironment();
    auto scanner = GlobalSymbolScanner(environment);

    auto ast1 = GenerateAST("module a; import b; foo: func = (){}");
    auto ast2 = GenerateAST("module b; export goo: func = (a:i32){}");
    ast1->Accept(&scanner);
    ast2->Accept(&scanner);

    auto foo = environment.FindSymbol("a", "foo");
    ASSERT_FALSE(foo.is_public);
    ASSERT_EQ(foo.scope_level, 0);
    ASSERT_EQ(foo.declaration->DeclType().ToString(), "[()]");

    auto goo = environment.FindSymbol("a", "goo");
    ASSERT_TRUE(goo.is_public);
    ASSERT_EQ(goo.scope_level, 0);
    ASSERT_EQ(goo.declaration->DeclType().ToString(), "[(in i32)]");
}

TEST(GlobalSymbolScanner, ThreeFilesPrivateImport)
{
    auto environment = ProgramEnvironment();
    auto scanner = GlobalSymbolScanner(environment);

    auto ast1 = GenerateAST("module a; import b;");
    auto ast2 = GenerateAST("module b; import c;");
    auto ast3 = GenerateAST("module c; foo: func = (){}");
    ast1->Accept(&scanner);
    ast2->Accept(&scanner);
    ast3->Accept(&scanner);

    // "foo" should not be visible outside module "c".
    EXPECT_THROW(environment.FindSymbol("a", "foo"), std::exception);
    EXPECT_THROW(environment.FindSymbol("b", "foo"), std::exception);
}

TEST(GlobalSymbolScanner, TwoFilesOneModule)
{
    auto environment = ProgramEnvironment();
    auto scanner = GlobalSymbolScanner(environment);

    auto ast1 = GenerateAST("module a; foo: func = (){}");
    auto ast2 = GenerateAST("module a; goo: func = (a: i32){}");
    ast1->Accept(&scanner);
    ast2->Accept(&scanner);

    // "foo" should not be visible outside module "c".
    EXPECT_EQ(environment.FindSymbol("a", "foo").declaration->DeclType().ToString(), "[()]");
    EXPECT_EQ(environment.FindSymbol("a", "goo").declaration->DeclType().ToString(), "[(in i32)]");
}

TEST(GlobalSymbolScanner, InvalidImport)
{
    auto environment = ProgramEnvironment();
    auto scanner = GlobalSymbolScanner(environment);

    auto ast = GenerateAST("module a; import b;");
    ast->Accept(&scanner);

    EXPECT_THROW(environment.ValidateModuleDependency(), SemanticError);
}

TEST(GlobalSymbolScanner, ODRViolation)
{
    auto environment = ProgramEnvironment();
    auto scanner = GlobalSymbolScanner(environment);

    auto ast = GenerateAST("module a; foo: func = (){} foo: struct = { x: str; }");

    EXPECT_THROW(ast->Accept(&scanner), SemanticError);
}

void ExpectTypeCheckSuccess(std::string&& source_file)
{
    auto environment = ProgramEnvironment();
    auto scanner = GlobalSymbolScanner(environment);
    auto type_checker = TypeChecker(environment);

    auto ast = GenerateAST(std::move(source_file));
    ast->Accept(&scanner);

    EXPECT_NO_THROW(ast->Accept(&type_checker));
}

void ExpectTypeCheckFailure(std::string&& source_file, std::string_view expected_error_message)
{
    auto environment = ProgramEnvironment();
    auto scanner = GlobalSymbolScanner(environment);
    auto type_checker = TypeChecker(environment);

    auto ast = GenerateAST(std::move(source_file));
    ast->Accept(&scanner);

    EXPECT_THROW(
        try
        {
            ast->Accept(&type_checker);
        }
        catch (const SemanticError& e)
        {
            ASSERT_EQ(std::string_view(e.what()), expected_error_message);
            throw;
        },
        SemanticError
    );
}

TEST(TypeChecker, ValidMemberTypes)
{
    auto source = 
        "module a;\n"
        "A: struct = {\n"
        "    m1: i32;\n"
        "    m2: B[2][3];\n"
        "}\n"
        "B: struct = {\n"
        "    m1: str;\n"
        "    m2: bool;\n"
        "}\n";
    ExpectTypeCheckSuccess(source);
}

TEST(TypeChecker, InvalidMemberType)
{
    auto source =
        "module a;\n"
        "A: struct = {\n"
        "    m1: i32;\n"
        "    m2: B;\n"
        "}\n";
    auto expected_error =
        "[Semantic Error][Ln 4, Col 5] member variable \"m2\" of struct \"A\" tried to use invalid type \"B\"";
    ExpectTypeCheckFailure(source, expected_error);
}

TEST(TypeChecker, InvalidMemberTypeZeroArraySize)
{
    auto source =
        "module a;\n"
        "A: struct = {\n"
        "    m: i32[0];\n"
        "}\n";
    auto expected_error =
        "[Semantic Error][Ln 3, Col 5] member variable \"m\" of struct \"A\" tried to use invalid type \"i32[0]\"";
    ExpectTypeCheckFailure(source, expected_error);
}

TEST(TypeChecker, ValidPrimitiveTypeVarDecl)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: i32 = 0;"
        "    j: str = \"hello\";\n"
        "}\n";
    ExpectTypeCheckSuccess(source);
}

TEST(TypeChecker, InvalidPrimitiveTypeVarDecl)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: i32 = \"hello\";"
        "}\n";
    auto expected_error =
        "[Semantic Error][Ln 3, Col 5] implicit conversion from base type \"str\" to \"i32\" is not allowed";
    ExpectTypeCheckFailure(source, expected_error);
}

TEST(TypeChecker, ValidArrayVarDecl)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: bool[2] = {true, false};\n"
        "    j: i32[2][2] = {{1, 2}, {3, 4}};\n"
        "    k: [(i32)];\n"
        "}\n";
    ExpectTypeCheckSuccess(source);
}

TEST(TypeChecker, ValidFuncTypeVar)
{
    auto source =
        "module a;\n"
        "add: func = (a: i32, b: i32) -> i32 {\n"
        "    return a + b;\n"
        "}\n"
        "main: func =() {\n"
        "    a: [()] = main;\n"
        "    b: [(i32, i32)->i32] = add;\n"
        "}\n";
    ExpectTypeCheckSuccess(source);
}

TEST(TypeChecker, InvalidFuncTypeVarUndefined)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    a: [(out str)] = foo;\n"
        "}\n";
    auto expected_error =
        "[Semantic Error][Ln 3, Col 5] trying to use undefined symbol \"foo\" in an expression";
    ExpectTypeCheckFailure(source, expected_error);
}

TEST(TypeChecker, InvalidFuncTypeVarTypeMismatch)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    a: [(out str)] = main;\n"
        "}\n";
    auto expected_error =
        "[Semantic Error][Ln 3, Col 5] implicit conversion from base type \"[()]\" to \"[(out str)]\" is not allowed";
    ExpectTypeCheckFailure(source, expected_error);
}

TEST(TypeChecker, InvalidArrayVarDeclMixedType)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: bool[2] = {true, 1};\n"
        "}\n";
    auto expected_error =
        "[Semantic Error][Ln 3, Col 5] trying to mix types \"bool\" and \"i32\" inside an initializer list";
    ExpectTypeCheckFailure(source, expected_error);
}

TEST(TypeChecker, InvalidArrayVarDeclMixedDimension)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: i32[2][2] = {{1, 2}, 1};\n"
        "}\n";
    auto expected_error =
        "[Semantic Error][Ln 3, Col 5] every element in an initializer list should have same dimension";
    ExpectTypeCheckFailure(source, expected_error);
}

// We should not reject initializer list with partial information,
// because we sometimes want to make big arrays which we cannot specify values one by one.
TEST(TypeChecker, ValidArrayVarDeclInsufficientInitializer)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: i32[100] = {1, 2};\n"
        "    j: i32[100][100] = {{1, 2}, {3}};\n"
        "}\n";
    ExpectTypeCheckSuccess(source);
}

// Initializer list should not exceed variable's array size.
TEST(TypeChecker, ValidArrayVarDeclTooLargeInitializer)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: i32[2] = {1, 2, 3};\n"
        "}\n";
    auto expected_error =
        "[Semantic Error][Ln 3, Col 5] array size of initializer's type \"i32[3]\" exceeds variable's type \"i32[2]\"";
    ExpectTypeCheckFailure(source, expected_error);
}

TEST(TypeChecker, InvalidIntVarDeclWithInitializerList)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: i32 = {1, 2, 3};\n"
        "}\n";
    auto expected =
        "[Semantic Error][Ln 3, Col 5] trying to assign expression of type \"i32[3]\" to \"i32\" type variable";
    ExpectTypeCheckFailure(source, expected);
}

TEST(TypeChecker, InvalidIntArrayWithDimensionMismatch)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: i32 = {1, 2, 3};\n"
        "}\n";
    auto expected =
        "[Semantic Error][Ln 3, Col 5] trying to assign expression of type \"i32[3]\" to \"i32\" type variable";
    ExpectTypeCheckFailure(source, expected);
}

TEST(TypeChecker, ValidVarDeclTypeCoercion)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: f32 = 1;\n"
        "}\n";
    ExpectTypeCheckSuccess(source);
}

TEST(TypeChecker, InvalidVarDeclTypeCoercion)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: i32 = 1.0;\n"
        "}\n";
    auto expected =
        "[Semantic Error][Ln 3, Col 5] implicit conversion from base type \"f32\" to \"i32\" is not allowed";
    ExpectTypeCheckFailure(source, expected);
}

TEST(TypeChecker, ValidVarDeclInitWithVar)
{
    auto source =
        "module a;\n"
        "main: func =() {\n"
        "    i: f32;\n"
        "    j: f32 = i;\n"
        "}\n";
    ExpectTypeCheckSuccess(source);
}