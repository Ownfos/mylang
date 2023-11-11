#include "lexer/DummyLexicalAnalyzer.h"
#include "parser/routine/ExprParser.h"
#include <gtest/gtest.h>

using namespace mylang;

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
        {TokenType::IntLiteral, "i"}
    });
    auto token_stream = std::make_shared<BufferedStream<Token>>(std::move(lexer));
    auto parser = ExprParser(token_stream);
    auto ast = parser.Parse();

    ASSERT_EQ(ast->ToString(), "(++ (-- (! i)))");
}