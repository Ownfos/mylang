#ifndef MYLANG_STMT_PARSER_H
#define MYLANG_STMT_PARSER_H

#include "parser/routine/IParseRoutine.h"
#include "parser/ast/stmt/Stmt.h"

namespace mylang
{

class StmtParser : public IParseRoutine<std::shared_ptr<Stmt>>
{
public:
    StmtParser(std::shared_ptr<BufferedStream<Token>> token_stream);

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
};

} // namespace mylang

#endif // MYLANG_STMT_PARSER_H
