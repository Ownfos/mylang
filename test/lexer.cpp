#include "lexer/DummyLexicalAnalyzer.h"
#include <gtest/gtest.h>

using namespace mylang;

TEST(DummyLexicalAnalyzer, EmptyList)
{
    auto l = DummyLexicalAnalyzer({});
    ASSERT_EQ(l.GetNextToken().type, TokenType::EndOfFile);
}

TEST(DummyLexicalAnalyzer, SingleToken)
{
    auto token = Token{
        .type = TokenType::Identifier,
        .lexeme = "foo"
    };
    auto l = DummyLexicalAnalyzer({token});
    ASSERT_EQ(l.GetNextToken().type, TokenType::Identifier);
    ASSERT_EQ(l.GetNextToken().type, TokenType::EndOfFile);
}