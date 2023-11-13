#ifndef MYLANG_STMT_PARSER_H
#define MYLANG_STMT_PARSER_H

#include "parser/routine/IParseRoutine.h"
#include "parser/ast/stmt/Stmt.h"
#include "parser/ast/expr/Expr.h"
#include "parser/type/Type.h"

namespace mylang
{

class StmtParser : public IParseRoutine<std::shared_ptr<Stmt>>
{
public:
    StmtParser(
        std::shared_ptr<BufferedStream<Token>> token_stream,
        std::shared_ptr<IParseRoutine<std::shared_ptr<Expr>>> expr_parser,
        std::shared_ptr<IParseRoutine<Type>> type_parser
    );

    virtual bool CanStartParsing() override;
    virtual std::shared_ptr<Stmt> Parse() override;

private:
    std::shared_ptr<Stmt> ParseCompoundStmt();
    std::shared_ptr<Stmt> ParseIfStmt();
    std::shared_ptr<Stmt> ParseForStmt();
    std::shared_ptr<Stmt> ParseWhileStmt();
    std::shared_ptr<Stmt> ParseJumpStmt();
    std::shared_ptr<Stmt> ParseVarDeclStmt();
    std::shared_ptr<Stmt> ParseExprStmt();

    std::shared_ptr<IParseRoutine<std::shared_ptr<Expr>>> m_expr_parser;
    std::shared_ptr<IParseRoutine<Type>> m_type_parser;
};

} // namespace mylang

#endif // MYLANG_STMT_PARSER_H
