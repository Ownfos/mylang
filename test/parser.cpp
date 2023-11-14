#include "lexer/DummyLexicalAnalyzer.h"
#include "parser/routine/ExprParser.h"
#include "parser/routine/StmtParser.h"
#include "parser/routine/TypeParser.h"
#include "parser/ast/visitor/TreePrinter.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace mylang;

TEST(TypeParser, Primitive)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::IntType, "i32"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = TypeParser(token_stream);
    auto type = parser.Parse();

    ASSERT_EQ(type.ToString(), "i32");
}

TEST(TypeParser, SimpleArray)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::IntType, "i32"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "100"},
        {TokenType::RightBracket, "]"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = TypeParser(token_stream);
    auto type = parser.Parse();

    ASSERT_EQ(type.ToString(), "i32[100]");
}

TEST(TypeParser, MultiDimensionArray)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::IntType, "i32"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "10"},
        {TokenType::RightBracket, "]"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "20"},
        {TokenType::RightBracket, "]"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = TypeParser(token_stream);
    auto type = parser.Parse();

    ASSERT_EQ(type.ToString(), "i32[10][20]");
}

TEST(TypeParser, SimpleCallable)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::LeftBracket, "["},
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"},
        {TokenType::RightBracket, "]"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = TypeParser(token_stream);
    auto type = parser.Parse();

    ASSERT_EQ(type.ToString(), "[()]");
}

TEST(TypeParser, SimpleCallableWithReturn)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::LeftBracket, "["},
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"},
        {TokenType::Arrow, "->"},
        {TokenType::BoolType, "bool"},
        {TokenType::RightBracket, "]"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = TypeParser(token_stream);
    auto type = parser.Parse();

    ASSERT_EQ(type.ToString(), "[() -> bool]");
}

TEST(TypeParser, TwoArgCallable)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::LeftBracket, "["},
        {TokenType::LeftParen, "("},
        {TokenType::IntType, "i32"},
        {TokenType::Comma, ","},
        {TokenType::StringType, "str"},
        {TokenType::RightParen, ")"},
        {TokenType::RightBracket, "]"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = TypeParser(token_stream);
    auto type = parser.Parse();

    ASSERT_EQ(type.ToString(), "[(in i32, in str)]");
}

TEST(ExprParser, Identifier)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "foo"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "foo");
}

TEST(ExprParser, BasicOperators)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::IntLiteral, "1"},
        {TokenType::Plus, "+"},
        {TokenType::IntLiteral, "2"},
        {TokenType::Multiply, "*"},
        {TokenType::FloatLiteral, "3.0"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "(1 + (2 * 3.0))");
}

TEST(ExprParser, MemberAccess)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "something"},
        {TokenType::Period, "."},
        {TokenType::Identifier, "member"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "something.member");
}

TEST(ExprParser, ConsecutiveMemberAcccess)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "something"},
        {TokenType::Period, "."},
        {TokenType::Identifier, "member1"},
        {TokenType::Period, "."},
        {TokenType::Identifier, "member2"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "something.member1.member2");
}

TEST(ExprParser, FuncCallNoArg)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "foo"},
        {TokenType::LeftParen, "("},
        {TokenType::RightParen, ")"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "foo()");
}

TEST(ExprParser, FuncCallSingleArg)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "foo"},
        {TokenType::LeftParen, "("},
        {TokenType::StringLiteral, "\"hello, world!\""},
        {TokenType::RightParen, ")"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "foo(\"hello, world!\")");
}

TEST(ExprParser, FuncCallMultipleArg)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "foo"},
        {TokenType::LeftParen, "("},
        {TokenType::IntLiteral, "1"},
        {TokenType::Comma, ","},
        {TokenType::IntLiteral, "2"},
        {TokenType::Comma, ","},
        {TokenType::IntLiteral, "3"},
        {TokenType::RightParen, ")"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "foo(1, 2, 3)");
}

TEST(ExprParser, ArrayAccess)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "arr"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "1"},
        {TokenType::RightBracket, "]"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "arr[1]");
}

TEST(ExprParser, ConsecutiveArrayAccess)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "arr"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "1"},
        {TokenType::RightBracket, "]"},
        {TokenType::LeftBracket, "["},
        {TokenType::IntLiteral, "2"},
        {TokenType::RightBracket, "]"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "arr[1][2]");
}

TEST(ExprParser, ExprWithParenthesis)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::LeftParen, "("},
        {TokenType::IntLiteral, "1"},
        {TokenType::Plus, "+"},
        {TokenType::IntLiteral, "2"},
        {TokenType::RightParen, ")"},
        {TokenType::Divide, "/"},
        {TokenType::IntLiteral, "3"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "((1 + 2) / 3)");
}

TEST(ExprParser, PrefixOperators)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Increment, "++"},
        {TokenType::Decrement, "--"},
        {TokenType::Not, "!"},
        {TokenType::Identifier, "i"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "(++(--(!i)))");
}

TEST(ExprParser, PostfixOperators)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "i"},
        {TokenType::Increment, "++"},
        {TokenType::Decrement, "--"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "((i++)--)");
}

TEST(ExprParser, Assignments)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "i"},
        {TokenType::Assign, "="},
        {TokenType::Identifier, "j"},
        {TokenType::PlusAssign, "+="},
        {TokenType::IntLiteral, "1"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "(i = (j += 1))");
}

TEST(ExprParser, LogicalOperators)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::BoolLiteral, "true"},
        {TokenType::And, "&&"},
        {TokenType::BoolLiteral, "false"},
        {TokenType::Or, "||"},
        {TokenType::BoolLiteral, "true"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "((true && false) || true)");
}

TEST(StmtParser, Expression)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::BoolLiteral, "true"},
        {TokenType::Semicolon, ";"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[ExprStmt]\n"
        "    [Literal]\n"
        "    - true\n";
    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, VariableDeclaration)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Identifier, "count"},
        {TokenType::Colon, ":"},
        {TokenType::IntType, "i32"},
        {TokenType::Assign, "="},
        {TokenType::IntLiteral, "0"},
        {TokenType::Semicolon, ";"},
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[VarDeclStmt]\n"
        "- name: count\n"
        "- type: i32\n"
        "    [Literal]\n    - 0\n";
    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, CompoundStatement)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);
    ASSERT_EQ(output.str(), "[CompoundStmt]\n");
}

TEST(StmtParser, IfStatement)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::If, "if"},
        {TokenType::LeftParen, "("},
        {TokenType::BoolLiteral, "true"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[IfStmt]\n"
        "    [Literal]\n"
        "    - true\n"
        "    [CompoundStmt]\n";
    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, IfElseStatement)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::If, "if"},
        {TokenType::LeftParen, "("},
        {TokenType::BoolLiteral, "true"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"},
        {TokenType::Else, "else"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"},
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[IfStmt]\n"
        "    [Literal]\n"
        "    - true\n"
        "    [CompoundStmt]\n"
        "    [CompoundStmt]\n";
    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, IfElseIfStatement)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
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
        {TokenType::RightBrace, "}"},
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[IfStmt]\n"
        "    [Literal]\n"
        "    - true\n"
        "    [CompoundStmt]\n"
        "    [IfStmt]\n"
        "        [Literal]\n"
        "        - false\n"
        "        [CompoundStmt]\n";
    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, WhileStatement)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::While, "while"},
        {TokenType::LeftParen, "("},
        {TokenType::BoolLiteral, "true"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[WhileStmt]\n"
        "    [Literal]\n"
        "    - true\n"
        "    [CompoundStmt]\n";
    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, ReturnStatement)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Return, "return"},
        {TokenType::Semicolon, ";"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[JumpStmt]\n"
        "- jump type: return\n";
    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, ReturnExprStatement)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::Return, "return"},
        {TokenType::IntLiteral, "0"},
        {TokenType::Semicolon, ";"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[JumpStmt]\n"
        "- jump type: return\n"
        "    [Literal]\n"
        "    - 0\n";
    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, SimpleForStatement)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
        {TokenType::For, "for"},
        {TokenType::LeftParen, "("},
        {TokenType::Semicolon, ";"},
        {TokenType::Semicolon, ";"},
        {TokenType::RightParen, ")"},
        {TokenType::LeftBrace, "{"},
        {TokenType::RightBrace, "}"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[ForStmt]\n"
        "    [CompoundStmt]\n";
    ASSERT_EQ(output.str(), expected);
}

TEST(StmtParser, ForStatementWithInit)
{
    auto lexer = std::make_unique<DummyLexicalAnalyzer>(std::vector<Token>{
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
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto expr_parser = std::make_shared<ExprParser>(token_stream);
    auto type_parser = std::make_shared<TypeParser>(token_stream);
    auto parser = StmtParser(token_stream, expr_parser, type_parser);
    auto ast = parser.Parse();

    auto output = std::ostringstream();
    auto printer = TreePrinter(output);
    ast->Accept(&printer);

    auto expected =
        "[ForStmt]\n"
        "    [VarDeclStmt]\n"
        "    - name: i\n"
        "    - type: i32\n"
        "        [Literal]\n"
        "        - 0\n"
        "    [CompoundStmt]\n";
    ASSERT_EQ(output.str(), expected);
}
