#include "parser/routine/StmtParser.h"
#include "parser/ast/stmt/CompoundStmt.h"
#include "parser/ast/stmt/IfStmt.h"
#include "parser/ast/stmt/WhileStmt.h"
#include "parser/ast/stmt/ForStmt.h"
#include "parser/ast/stmt/JumpStmt.h"
#include "parser/ast/stmt/VarDeclStmt.h"
#include "parser/ast/stmt/ExprStmt.h"

namespace mylang
{

std::set<TokenType> JumpTypes()
{
    return {
        TokenType::Return,
        TokenType::Break,
        TokenType::Continue
    };
}

bool IsFirstOfJumpStmt(TokenType type)
{
    return JumpTypes().count(type) > 0;
}

StmtParser::StmtParser(
    std::shared_ptr<BufferedStream<Token>> token_stream,
    std::shared_ptr<IParseRoutine<std::shared_ptr<Expr>>> expr_parser,
    std::shared_ptr<IParseRoutine<Type>> type_parser
)
    : IParseRoutine(token_stream)
    , m_expr_parser(expr_parser)
    , m_type_parser(type_parser)
{}

bool StmtParser::CanStartParsing()
{
    auto type = Peek();
    return
        type == TokenType::LeftBrace ||
        type == TokenType::For ||
        type == TokenType::While ||
        type == TokenType::If ||
        IsFirstOfJumpStmt(type) ||
        m_expr_parser->CanStartParsing();
}

// stmt ::= expr ";"
//       | var-decl ";"
//       | compound-stmt
//       | if-stmt
//       | for-stmt
//       | while-stmt
//       | jump-stmt
std::shared_ptr<Stmt> StmtParser::Parse()
{
    switch(Peek())
    {
    case TokenType::LeftBrace:
        return ParseCompoundStmt();
    case TokenType::If:
        return ParseIfStmt();
    case TokenType::For:
        return ParseForStmt();
    case TokenType::While:
        return ParseWhileStmt();
    default:
        // Note: we need two lookaheads to distinguish the following nonterminals
        // - "identifier : ..." => var-decl
        // - "identifier = ..." => expr
        if (Peek(0) == TokenType::Identifier && Peek(1) == TokenType::Colon)
        {
            return ParseVarDeclStmt();
        }
        else if (m_expr_parser->CanStartParsing())
        {
            return ParseExprStmt();
        }
        else
        {
            return ParseJumpStmt();
        }
    }
}

// compound-stmt ::= "{" stmt* "}"
std::shared_ptr<Stmt> StmtParser::ParseCompoundStmt()
{
    auto statements = std::vector<std::shared_ptr<Stmt>>{};
    Accept(TokenType::LeftBrace);
    while (CanStartParsing())
    {
        statements.push_back(Parse());
    }
    Accept(TokenType::RightBrace);
    return std::make_shared<CompoundStmt>(statements);
}

// if-stmt ::= "if" "(" expr ")" compound-stmt ("else" (if-stmt | compound-stmt))?
std::shared_ptr<Stmt> StmtParser::ParseIfStmt()
{
    Accept(TokenType::If);
    Accept(TokenType::LeftParen);
    auto condition = m_expr_parser->Parse();
    Accept(TokenType::RightParen);
    auto then_branch = ParseCompoundStmt();

    // Parse optional 'else' or 'else-if' branch.
    auto else_branch = std::shared_ptr<Stmt>{};
    if (OptionalAccept(TokenType::Else))
    {
        // Case 1) else if (...) {...}
        if (Peek() == TokenType::If)
        {
            else_branch = ParseIfStmt();
        }
        // Case 2) else {...}
        else
        {
            else_branch = ParseCompoundStmt();
        }
    }

    return std::make_shared<IfStmt>(condition, then_branch, else_branch);
}

std::shared_ptr<Stmt> StmtParser::ParseForStmt()
{
    // TODO: implement
    return {};
}

std::shared_ptr<Stmt> StmtParser::ParseWhileStmt()
{
    // TODO: implement
    return {};
}

std::shared_ptr<Stmt> StmtParser::ParseJumpStmt()
{
    // TODO: implement
    return {};
}

// var-decl-stmt ::= var-decl ";"
// var-decl      ::= identifier ":" type "=" var-init
// var-init      ::= expr | "{" var-init ("," var-init)* "}"
std::shared_ptr<Stmt> StmtParser::ParseVarDeclStmt()
{
    auto id = Accept(TokenType::Identifier);
    Accept(TokenType::Colon);
    auto type = m_type_parser->Parse();
    Accept(TokenType::Assign);
    auto expr = m_expr_parser->Parse();
    Accept(TokenType::Semicolon);
    return std::make_shared<VarDeclStmt>(id, type, expr);
    // TODO: implement initializer list parsing
}

// expr-stmt ::= expr ";"
std::shared_ptr<Stmt> StmtParser::ParseExprStmt()
{
    auto expr = m_expr_parser->Parse();
    Accept(TokenType::Semicolon);
    return std::make_shared<ExprStmt>(expr);
}

} // namespace mylang
