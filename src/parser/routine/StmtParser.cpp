#include "parser/routine/StmtParser.h"
#include "parser/ast/stmt/CompoundStmt.h"
#include "parser/ast/stmt/IfStmt.h"
#include "parser/ast/stmt/WhileStmt.h"
#include "parser/ast/stmt/ForStmt.h"
#include "parser/ast/stmt/JumpStmt.h"
#include "parser/ast/stmt/VarDeclStmt.h"
#include "parser/ast/stmt/ExprStmt.h"
#include "parser/ast/varinit/VarInitExpr.h"
#include "parser/ast/varinit/VarInitList.h"

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

bool StmtParser::CanStartParsingVarDecl()
{
    // Note: we need two lookaheads to distinguish the following nonterminals
    // - "identifier : ..." => var-decl
    // - "identifier = ..." => expr
    return Peek(0) == TokenType::Identifier && Peek(1) == TokenType::Colon;
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
    try
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
            if (CanStartParsingVarDecl())
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
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "stmt ::= expr-stmt | var-decl-stmt | if-stmt | for-stmt | while-stmt | jump-stmt");
    }
    
}

// compound-stmt ::= "{" stmt* "}"
std::shared_ptr<Stmt> StmtParser::ParseCompoundStmt()
{
    try
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
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "compound-stmt ::= \"{\" stmt* \"}\"");
    }
}

// if-stmt ::= "if" "(" expr ")" compound-stmt ("else" (if-stmt | compound-stmt))?
std::shared_ptr<Stmt> StmtParser::ParseIfStmt()
{
    try
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
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "if-stmt ::= \"if\" \"(\" expr \")\" compound-stmt (\"else\" (if-stmt | compound-stmt))?");
    }
}

// for-stmt ::= "for" "(" (var-decl | expr)? ";" expr? ";" expr? ")" compound-stmt
std::shared_ptr<Stmt> StmtParser::ParseForStmt()
{
    try
    {
        Accept(TokenType::For);
        Accept(TokenType::LeftParen);

        // Optional initializer ::= (var-decl | expr)? ";"
        auto initializer = std::shared_ptr<Stmt>{};
        if (CanStartParsingVarDecl())
        {
            initializer = ParseVarDeclStmt();
        }
        else if (m_expr_parser->CanStartParsing())
        {
            initializer = ParseExprStmt();
        }
        else
        {
            // Note: ParseVarDeclStmt() and ParseExprStmt() accept ";"
            // at the end, so we should only accept ";" if neither existed.
            Accept(TokenType::Semicolon);
        }

        // Optional condition expr
        auto condition = std::shared_ptr<Expr>{};
        if (m_expr_parser->CanStartParsing())
        {
            condition = m_expr_parser->Parse();
        }
        Accept(TokenType::Semicolon);

        // Optional increment expr
        auto increment_expr = std::shared_ptr<Expr>{};
        if (m_expr_parser->CanStartParsing())
        {
            increment_expr = m_expr_parser->Parse();
        }

        Accept(TokenType::RightParen);

        auto body = ParseCompoundStmt();

        return std::make_shared<ForStmt>(
            initializer,
            condition,
            increment_expr,
            body
        );
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "for-stmt ::= \"for\" \"(\" (var-decl | expr)? \";\" expr? \";\" expr? \")\" compound-stmt");
    }
}

// while-stmt ::= "while" "(" expr ")" compound-stmt
std::shared_ptr<Stmt> StmtParser::ParseWhileStmt()
{
    try
    {
        Accept(TokenType::While);
        Accept(TokenType::LeftParen);
        auto condition = m_expr_parser->Parse();
        Accept(TokenType::RightParen);
        auto body = ParseCompoundStmt();

        return std::make_shared<WhileStmt>(condition, body);
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "while-stmt ::= \"while\" \"(\" expr \")\" compound-stmt");
    }
}

// jump-stmt ::= ("return" expr? | "break" | "continue") ";"
std::shared_ptr<Stmt> StmtParser::ParseJumpStmt()
{
    try
    {
        auto jump_type = AcceptOneOf(JumpTypes());

        // Optional return value expr
        auto expr = std::shared_ptr<Expr>{};
        if (jump_type.type == TokenType::Return && m_expr_parser->CanStartParsing())
        {
            expr = m_expr_parser->Parse();
        }

        Accept(TokenType::Semicolon);

        return std::make_shared<JumpStmt>(jump_type, expr);
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "jump-stmt :: (\"return\" expr? | \"break\" | \"continue\") \";\"");
    }
}

// var-decl-stmt ::= var-decl ";"
// var-decl      ::= identifier ":" type "=" var-init
std::shared_ptr<Stmt> StmtParser::ParseVarDeclStmt()
{
    try
    {
        auto id = Accept(TokenType::Identifier);
        Accept(TokenType::Colon);
        auto type = m_type_parser->Parse();
        Accept(TokenType::Assign);
        auto initializer = ParseVarInit();
        Accept(TokenType::Semicolon);
        return std::make_shared<VarDeclStmt>(id, type, initializer);
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "var-decl-stmt ::= identifier  \":\" type \"=\" var-init");
    }
}

// var-init ::= expr | "{" var-init ("," var-init)* "}"
std::shared_ptr<VarInit> StmtParser::ParseVarInit()
{
    try
    {
        // "{" var-init ("," var-init)* "}"
        if (OptionalAccept(TokenType::LeftBrace))
        {
            auto initializer_list = std::vector<std::shared_ptr<VarInit>>{};
            initializer_list.push_back(ParseVarInit());
            while (OptionalAccept(TokenType::Comma))
            {
                initializer_list.push_back(ParseVarInit());
            }
            Accept(TokenType::RightBrace);

            return std::make_shared<VarInitList>(initializer_list);
        }
        // expr
        else
        {
            return std::make_shared<VarInitExpr>(m_expr_parser->Parse());
        }
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "var-init ::= expr | \"{\" var-init (\",\" var-init)* \"}\"");
    }
}

// expr-stmt ::= expr ";"
std::shared_ptr<Stmt> StmtParser::ParseExprStmt()
{
    try
    {
        auto expr = m_expr_parser->Parse();
        Accept(TokenType::Semicolon);
        return std::make_shared<ExprStmt>(expr);
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "expr-stmt :: expr \";\"");
    }
}

} // namespace mylang
