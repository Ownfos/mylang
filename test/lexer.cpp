#include "lexer/DummyLexicalAnalyzer.h"
#include "lexer/LexicalAnalyzer.h"
#include "file/DummySourceFile.h"
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
    auto lexer = DummyLexicalAnalyzer({token});
    ASSERT_EQ(lexer.GetNextToken().type, TokenType::Identifier);
    ASSERT_EQ(lexer.GetNextToken().type, TokenType::EndOfFile);
}

TEST(LexicalAnalyzer, EmptyList)
{
    auto source_file = std::make_unique<DummySourceFile>("");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 1}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, If)
{
    auto source_file = std::make_unique<DummySourceFile>("if");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::If,
        .lexeme = "if",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 2}
    };
    ASSERT_EQ(token, expected);
}