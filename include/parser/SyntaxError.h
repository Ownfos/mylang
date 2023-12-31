#ifndef MYLANG_SYNTAX_ERROR_H
#define MYLANG_SYNTAX_ERROR_H

#include "lexer/Token.h"
#include <exception>
#include <set>

namespace mylang
{

class SyntaxError : public std::exception
{
public:
    SyntaxError(const SourcePos& location, std::string_view message);
    
    const SourcePos& where() const;
    virtual const char* what() const override;

private:
    SourcePos m_location;
    std::string m_message;
};

// Baseclass for errors generated during syntax analysis.
// SyntaxAnalyzer catches all ParseRoutineError to generate SyntaxError.
class ParseRoutineError
{
public:
    virtual const SourcePos& Location() const = 0;
    virtual std::string_view Description() const = 0;
};

// This error occurs when IParseRoutine<T>::Accept()
// or IParseRoutine<T>::AcceptOneOf() fails.
class UnexpectedTokenError : public ParseRoutineError
{
public:
    UnexpectedTokenError(
        const Token& token,
        const std::set<TokenType>& expected_types
    );
    
    virtual const SourcePos& Location() const override;
    virtual std::string_view Description() const override;

private:
    std::string ExpectedTypeDescriptor() const;
    std::string TokenDescriptor() const;

    Token m_token;
    std::set<TokenType> m_expected_types;
    std::string m_message;
};

// A nested exception used to provide where the pattern mismatch happened.
//
// Since PatternMismatchError can nest a PatternMismatchError,
// the overall Description() will involve full call stack information.
//
// example)
// failed to match pattern: aaa ::= bbb
// -> failed to match pattern: bbb ::= ccc
// -> failed to match pattern: ccc ::= identifier
// -> unexpected token ...
class PatternMismatchError : public ParseRoutineError
{
public:
    PatternMismatchError(
        const ParseRoutineError& error,
        std::string_view pattern
    );

    virtual const SourcePos& Location() const override;
    virtual std::string_view Description() const override;

private:
    SourcePos m_location;
    std::string m_message;
};

class LeftoverTokenError : public ParseRoutineError
{
public:
    LeftoverTokenError(const Token& token);

    virtual const SourcePos& Location() const override;
    virtual std::string_view Description() const override;

private:
    SourcePos m_location;
    std::string m_message;
};

} // namespace mylang


#endif // MYLANG_SYNTAX_ERROR_H
