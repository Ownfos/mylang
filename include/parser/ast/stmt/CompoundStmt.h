#ifndef MYLANG_COMPOUND_STMT_H
#define MYLANG_COMPOUND_STMT_H

#include "parser/ast/stmt/Stmt.h"
#include <memory>
#include <vector>

namespace mylang
{

class CompoundStmt : public Stmt
{
public:
    CompoundStmt(const std::vector<std::shared_ptr<Stmt>>& statements);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;

    const std::vector<std::shared_ptr<Stmt>>& Statements() const;

private:
    std::vector<std::shared_ptr<Stmt>> m_statements;
};

} // namespace mylang


#endif // MYLANG_COMPOUND_STMT_H
