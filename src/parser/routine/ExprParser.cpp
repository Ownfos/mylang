#include "parser/routine/ExprParser.h"
#include "parser/ast/expr/BinaryExpr.h"
#include "parser/ast/expr/PrefixExpr.h"
#include "parser/ast/expr/PostfixExpr.h"
#include "parser/ast/expr/FuncCallExpr.h"
#include "parser/ast/expr/MemberAccessExpr.h"
#include "parser/ast/expr/ArrayAccessExpr.h"
#include "parser/ast/expr/Identifier.h"
#include "parser/ast/expr/Literal.h"

namespace mylang
{

std::set<TokenType> PrefixOps()
{
    return {
        TokenType::Not,
        TokenType::Plus,
        TokenType::Minus,
        TokenType::Increment,
        TokenType::Decrement
    };
}

bool IsFirstOfPrefixExpr(TokenType type)
{
    return PrefixOps().count(type) > 0;
}

std::set<TokenType> LiteralTypes()
{
    return {
        TokenType::IntLiteral,
        TokenType::FloatLiteral,
        TokenType::BoolLiteral,
        TokenType::StringLiteral
    };
}

bool IsLiteral(TokenType type)
{
    return LiteralTypes().count(type) > 0;
}

std::set<TokenType> AssignmentOps()
{
    return {
        TokenType::Assign,
        TokenType::PlusAssign,
        TokenType::MinusAssign,
        TokenType::MultiplyAssign,
        TokenType::DivideAssign
    };
}

bool IsAssignmentOperator(TokenType type)
{
    return AssignmentOps().count(type) > 0;
}

std::set<TokenType> ComparisonOps()
{
    return {
        TokenType::Equal,
        TokenType::NotEqual,
        TokenType::Less,
        TokenType::LessEqual,
        TokenType::Greater,
        TokenType::GreaterEqual
    };
}

ExprParser::ExprParser(std::shared_ptr<BufferedStream<Token>> token_stream)
    : IParseRoutine(token_stream)
{}

bool ExprParser::CanStartParsing()
{
    auto type = Peek();
    return
        type == TokenType::Identifier ||
        type == TokenType::LeftParen ||
        IsLiteral(type) ||
        IsFirstOfPrefixExpr(type);
}

// expr      ::= identifier assign-op expr | or-expr
// assign-op ::= "=" | "+=" | "-=" | "*=" | "/="
std::shared_ptr<Expr> ExprParser::Parse()
{
    try
    {
        // Since primary-expr also starts with identifier,
        // we should have two lookaheads to confirm an assignment.
        if (Peek(0) == TokenType::Identifier && IsAssignmentOperator(Peek(1)))
        {
            return ParseAssignExpr();
        }
        else
        {
            return ParseOrExpr();
        }
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "expr");
    }
}

// assign-expr ::= identifier assign-op expr
std::shared_ptr<Expr> ExprParser::ParseAssignExpr()
{
    auto lhs = std::make_shared<Identifier>(
        Accept(TokenType::Identifier)
    );
    auto op = AcceptOneOf(AssignmentOps());
    auto rhs = Parse();

    return std::make_shared<BinaryExpr>(op, lhs, rhs);
}

// or-expr ::= and-expr ("||" and-expr)*
std::shared_ptr<Expr> ExprParser::ParseOrExpr()
{
    auto expr = ParseAndExpr();
    while (auto op = OptionalAccept(TokenType::Or))
    {
        auto rhs = ParseAndExpr();
        expr = std::make_shared<BinaryExpr>(op.value(), expr, rhs);
    }
    return expr;
}

// and-expr ::= compare-expr ("&&" compare-expr)*
std::shared_ptr<Expr> ExprParser::ParseAndExpr()
{
    auto expr = ParseCompareExpr();
    while (auto op = OptionalAccept(TokenType::And))
    {
        auto rhs = ParseCompareExpr();
        expr = std::make_shared<BinaryExpr>(op.value(), expr, rhs);
    }
    return expr;
}

// compare-expr ::= add-expr (compare-op add-expr)*
std::shared_ptr<Expr> ExprParser::ParseCompareExpr()
{
    auto expr = ParseAddExpr();
    while (auto op = OptionalAcceptOneOf(ComparisonOps()))
    {
        auto rhs = ParseAddExpr();
        expr = std::make_shared<BinaryExpr>(op.value(), expr, rhs);
    }
    return expr;
}

// add-expr ::= mult-expr (("+" | "-") mult-expr)*
std::shared_ptr<Expr> ExprParser::ParseAddExpr()
{
    auto expr = ParseMultExpr();
    while (auto op = OptionalAcceptOneOf({TokenType::Plus, TokenType::Minus}))
    {
        auto rhs = ParseMultExpr();
        expr = std::make_shared<BinaryExpr>(op.value(), expr, rhs);
    }
    return expr;
}

// mult-expr ::= prefix-expr (("*" | "/") prefix-expr)*
std::shared_ptr<Expr> ExprParser::ParseMultExpr()
{
    auto expr = ParsePrefixExpr();
    while (auto op = OptionalAcceptOneOf({TokenType::Multiply, TokenType::Divide}))
    {
        auto rhs = ParsePrefixExpr();
        expr = std::make_shared<BinaryExpr>(op.value(), expr, rhs);
    }
    return expr;
}

// prefix-expr ::= prefix-op* postfix-expr
std::shared_ptr<Expr> ExprParser::ParsePrefixExpr()
{
    if (auto op = OptionalAcceptOneOf(PrefixOps()))
    {
        auto expr = ParsePrefixExpr();
        return std::make_shared<PrefixExpr>(op.value(), expr);
    }
    else
    {
        return ParsePostfixExpr();
    }
}

// postfix-expr ::= primary-expr postfix-op*
// postfix-op   ::= "++" | "--" | member-access | func-call | array-index
std::shared_ptr<Expr> ExprParser::ParsePostfixExpr()
{
    // Parse primary-expr first.
    auto expr = ParsePrimaryExpr();

    // Parse following postfix-op by nesting expr.
    while (true)
    {
        // "++"" | "--"
        if (auto op = OptionalAcceptOneOf({TokenType::Increment, TokenType::Decrement}))
        {
            expr = std::make_shared<PostfixExpr>(op.value(), expr);
        }
        // member-access ::= "." identifier
        else if (OptionalAccept(TokenType::Period))
        {
            auto id = Accept(TokenType::Identifier);
            expr = std::make_shared<MemberAccessExpr>(expr, id);
        }
        // array-index :: "[" expr "]"
        else if (OptionalAccept(TokenType::LeftBracket))
        {
            auto index = Parse();
            Accept(TokenType::RightBracket);
            expr = std::make_shared<ArrayAccessExpr>(expr, index);
        }
        // func-call ::= "(" arg-list? ")"
        // arg-list  ::= expr ("," expr)*
        else if(OptionalAccept(TokenType::LeftParen))
        {
            auto arg_list = std::vector<std::shared_ptr<Expr>>{};
            if (CanStartParsing())
            {
                arg_list.push_back(Parse());
                while (OptionalAccept(TokenType::Comma))
                {
                    arg_list.push_back(Parse());
                }
            }
            Accept(TokenType::RightParen);
            return std::make_shared<FuncCallExpr>(expr, arg_list);
        }
        // Finished parsing postfix expressions!
        else
        {
            break;
        }
    }

    return expr;
}

// primary-expr ::= literal | identifier | "(" expr ")"
std::shared_ptr<Expr> ExprParser::ParsePrimaryExpr()
{
    if (auto literal = OptionalAcceptOneOf(LiteralTypes()))
    {
        return std::make_shared<Literal>(literal.value());
    }
    else if (auto id = OptionalAccept(TokenType::Identifier))
    {
        return std::make_shared<Identifier>(id.value());
    }
    else
    {
        Accept(TokenType::LeftParen);
        auto expr = Parse();
        Accept(TokenType::RightParen);

        return expr;
    }
}

} // namespace mylang
