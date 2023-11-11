#ifndef MYLANG_EXPR_PARSER_H
#define MYLANG_EXPR_PARSER_H

#include "parser/routine/IParseRoutine.h"
#include "parser/ast/expr/Expr.h"

namespace mylang
{

class ExprParser : public IParseRoutine<std::shared_ptr<Expr>>
{
public:
    ExprParser(std::shared_ptr<BufferedStream<Token>> token_stream);

    virtual bool CanStartParsing() override;
    virtual std::shared_ptr<Expr> Parse() override;

private:
    std::shared_ptr<Expr> ParseAssignExpr();
    std::shared_ptr<Expr> ParseOrExpr();
    std::shared_ptr<Expr> ParseAndExpr();
    std::shared_ptr<Expr> ParseCompareExpr();
    std::shared_ptr<Expr> ParseAddExpr();
    std::shared_ptr<Expr> ParseMultExpr();
    std::shared_ptr<Expr> ParsePrefixExpr();
    std::shared_ptr<Expr> ParsePostfixExpr();
    std::shared_ptr<Expr> ParsePrimaryExpr();
};

} // namespace mylang

#endif // MYLANG_EXPR_PARSER_H
