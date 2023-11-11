#include "parser/routine/StmtParser.h"
#include "parser/ast/stmt/IfStmt.h"
#include "parser/ast/stmt/WhileStmt.h"
#include "parser/ast/stmt/ForStmt.h"
#include "parser/ast/stmt/JumpStmt.h"
#include "parser/ast/stmt/VarDeclStmt.h"
#include "parser/ast/stmt/ExprStmt.h"

namespace mylang
{

StmtParser::StmtParser(std::shared_ptr<BufferedStream<Token>> token_stream)
    : IParseRoutine(token_stream)
{}

bool StmtParser::CanStartParsing()
{
    // TODO: implement
    return false;
}

std::shared_ptr<Stmt> StmtParser::Parse()
{
    // TODO: implement
    return {};
}

std::shared_ptr<Stmt> StmtParser::ParseCompoundStmt()
{
    // TODO: implement
    return {};
}

std::shared_ptr<Stmt> StmtParser::ParseIfStmt()
{
    // TODO: implement
    return {};
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

std::shared_ptr<Stmt> StmtParser::ParseVarDeclStmt()
{
    // TODO: implement
    return {};
}

std::shared_ptr<Stmt> StmtParser::ParseExprStmt()
{
    // TODO: implement
    return {};
}

} // namespace mylang
