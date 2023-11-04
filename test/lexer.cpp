#include "lexer/DummyLexicalAnalyzer.h"
#include "lexer/LexicalAnalyzer.h"
#include "lexer/LexicalError.h"
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

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 3},
        .end_pos = SourcePos{.line = 1, .column = 3}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, TwoIfWithWhitespaces)
{
    auto source_file = std::make_unique<DummySourceFile>("  \n if\t \r\n  if   ");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::If,
        .lexeme = "if",
        .start_pos = SourcePos{.line = 2, .column = 2},
        .end_pos = SourcePos{.line = 2, .column = 3}
    };
    
    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::If,
        .lexeme = "if",
        .start_pos = SourcePos{.line = 3, .column = 3},
        .end_pos = SourcePos{.line = 3, .column = 4}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 3, .column = 8},
        .end_pos = SourcePos{.line = 3, .column = 8}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, Identifier)
{
    auto source_file = std::make_unique<DummySourceFile>("_foo123");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::Identifier,
        .lexeme = "_foo123",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 7}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 8},
        .end_pos = SourcePos{.line = 1, .column = 8}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, ErrorTokens)
{
    auto source_file = std::make_unique<DummySourceFile>("#$%");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::Error,
        .lexeme = "#",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 1}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Error,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 2},
        .end_pos = SourcePos{.line = 1, .column = 2}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Error,
        .lexeme = "%",
        .start_pos = SourcePos{.line = 1, .column = 3},
        .end_pos = SourcePos{.line = 1, .column = 3}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 4},
        .end_pos = SourcePos{.line = 1, .column = 4}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, NumericLiterals)
{
    auto source_file = std::make_unique<DummySourceFile>("123 45.67");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::IntLiteral,
        .lexeme = "123",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 3}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::FloatLiteral,
        .lexeme = "45.67",
        .start_pos = SourcePos{.line = 1, .column = 5},
        .end_pos = SourcePos{.line = 1, .column = 9}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 10},
        .end_pos = SourcePos{.line = 1, .column = 10}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, BasicOperators)
{
    auto source_file = std::make_unique<DummySourceFile>("*/+-");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::Multiply,
        .lexeme = "*",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 1}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Divide,
        .lexeme = "/",
        .start_pos = SourcePos{.line = 1, .column = 2},
        .end_pos = SourcePos{.line = 1, .column = 2}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Plus,
        .lexeme = "+",
        .start_pos = SourcePos{.line = 1, .column = 3},
        .end_pos = SourcePos{.line = 1, .column = 3}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Minus,
        .lexeme = "-",
        .start_pos = SourcePos{.line = 1, .column = 4},
        .end_pos = SourcePos{.line = 1, .column = 4}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 5},
        .end_pos = SourcePos{.line = 1, .column = 5}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, Brackets)
{
    auto source_file = std::make_unique<DummySourceFile>("(){}[]");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::LeftParen,
        .lexeme = "(",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 1}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::RightParen,
        .lexeme = ")",
        .start_pos = SourcePos{.line = 1, .column = 2},
        .end_pos = SourcePos{.line = 1, .column = 2}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::LeftBrace,
        .lexeme = "{",
        .start_pos = SourcePos{.line = 1, .column = 3},
        .end_pos = SourcePos{.line = 1, .column = 3}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::RightBrace,
        .lexeme = "}",
        .start_pos = SourcePos{.line = 1, .column = 4},
        .end_pos = SourcePos{.line = 1, .column = 4}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::LeftBracket,
        .lexeme = "[",
        .start_pos = SourcePos{.line = 1, .column = 5},
        .end_pos = SourcePos{.line = 1, .column = 5}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::RightBracket,
        .lexeme = "]",
        .start_pos = SourcePos{.line = 1, .column = 6},
        .end_pos = SourcePos{.line = 1, .column = 6}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 7},
        .end_pos = SourcePos{.line = 1, .column = 7}
    };
    ASSERT_EQ(token, expected);
}


TEST(LexicalAnalyzer, Comparision)
{
    auto source_file = std::make_unique<DummySourceFile>("< <= > >= = == ! !=");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::Less,
        .lexeme = "<",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 1}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::LessEqual,
        .lexeme = "<=",
        .start_pos = SourcePos{.line = 1, .column = 3},
        .end_pos = SourcePos{.line = 1, .column = 4}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Greater,
        .lexeme = ">",
        .start_pos = SourcePos{.line = 1, .column = 6},
        .end_pos = SourcePos{.line = 1, .column = 6}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::GreaterEqual,
        .lexeme = ">=",
        .start_pos = SourcePos{.line = 1, .column = 8},
        .end_pos = SourcePos{.line = 1, .column = 9}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Assign,
        .lexeme = "=",
        .start_pos = SourcePos{.line = 1, .column = 11},
        .end_pos = SourcePos{.line = 1, .column = 11}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Equal,
        .lexeme = "==",
        .start_pos = SourcePos{.line = 1, .column = 13},
        .end_pos = SourcePos{.line = 1, .column = 14}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Not,
        .lexeme = "!",
        .start_pos = SourcePos{.line = 1, .column = 16},
        .end_pos = SourcePos{.line = 1, .column = 16}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::NotEqual,
        .lexeme = "!=",
        .start_pos = SourcePos{.line = 1, .column = 18},
        .end_pos = SourcePos{.line = 1, .column = 19}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 20},
        .end_pos = SourcePos{.line = 1, .column = 20}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, LogicalAndOr)
{
    auto source_file = std::make_unique<DummySourceFile>("& && | ||");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::Error,
        .lexeme = "&",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 1}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::And,
        .lexeme = "&&",
        .start_pos = SourcePos{.line = 1, .column = 3},
        .end_pos = SourcePos{.line = 1, .column = 4}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Error,
        .lexeme = "|",
        .start_pos = SourcePos{.line = 1, .column = 6},
        .end_pos = SourcePos{.line = 1, .column = 6}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::Or,
        .lexeme = "||",
        .start_pos = SourcePos{.line = 1, .column = 8},
        .end_pos = SourcePos{.line = 1, .column = 9}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 10},
        .end_pos = SourcePos{.line = 1, .column = 10}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, SingleLineComment)
{
    auto source_file = std::make_unique<DummySourceFile>("  //for while do 123 4.56  \r\ntrue //\"hello, world\"");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::BoolLiteral,
        .lexeme = "true",
        .start_pos = SourcePos{.line = 2, .column = 1},
        .end_pos = SourcePos{.line = 2, .column = 4}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 2, .column = 22},
        .end_pos = SourcePos{.line = 2, .column = 22}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, StringLiteral)
{
    auto source_file = std::make_unique<DummySourceFile>("\"hello, world!\"");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::StringLiteral,
        .lexeme = "\"hello, world!\"",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 15}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 16},
        .end_pos = SourcePos{.line = 1, .column = 16}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, ValidEscapeSequence)
{
    auto source_file = std::make_unique<DummySourceFile>("\"\\n \\r \\t \\\\ \\\" \\'\"");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    auto token = lexer.GetNextToken();
    auto expected = Token{
        .type = TokenType::StringLiteral,
        .lexeme = "\"\\n \\r \\t \\\\ \\\" \\'\"",
        .start_pos = SourcePos{.line = 1, .column = 1},
        .end_pos = SourcePos{.line = 1, .column = 19}
    };
    ASSERT_EQ(token, expected);

    token = lexer.GetNextToken();
    expected = Token{
        .type = TokenType::EndOfFile,
        .lexeme = "$",
        .start_pos = SourcePos{.line = 1, .column = 20},
        .end_pos = SourcePos{.line = 1, .column = 20}
    };
    ASSERT_EQ(token, expected);
}

TEST(LexicalAnalyzer, InvalidEscapeSequence)
{
    auto source_file = std::make_unique<DummySourceFile>("\"\\a\"");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    EXPECT_THROW({lexer.GetNextToken();}, LexicalError);
}

TEST(LexicalAnalyzer, UnterminatedString)
{
    auto source_file = std::make_unique<DummySourceFile>("\"");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    EXPECT_THROW({lexer.GetNextToken();}, LexicalError);
}

TEST(LexicalAnalyzer, UnterminatedComment)
{
    auto source_file = std::make_unique<DummySourceFile>("/*asdf");
    auto lexer = LexicalAnalyzer(std::move(source_file));
    EXPECT_THROW({lexer.GetNextToken();}, LexicalError);
}