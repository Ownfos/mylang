#include "parser/ast/stmt/VarDeclStmt.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

VarDeclStmt::VarDeclStmt(const Token& name, const Type& type, std::shared_ptr<Expr> expr)
    : m_name(name)
    , m_type(type)
    , m_expr(expr)
{}

void VarDeclStmt::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
    
    visitor->IncreaseDepth();
    m_expr->Accept(visitor);
    visitor->DecreaseDepth();
}

const Token& VarDeclStmt::Name() const
{
    return m_name;
}

const Type& VarDeclStmt::TypeSpecifier() const
{
    return m_type;
}

} // namespace mylang
