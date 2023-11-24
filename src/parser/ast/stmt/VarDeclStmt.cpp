#include "parser/ast/stmt/VarDeclStmt.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

VarDeclStmt::VarDeclStmt(const Token& name, const Type& type, std::shared_ptr<VarInit> initializer)
    : m_name(name)
    , m_type(type)
    , m_initializer(initializer)
{}

void VarDeclStmt::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    m_initializer->Accept(visitor);
    visitor->PostorderVisit(this);
}

const Token& VarDeclStmt::Name() const
{
    return m_name;
}

const Type& VarDeclStmt::DeclType() const
{
    return m_type;
}

const VarInit* VarDeclStmt::Initializer() const
{
    return m_initializer.get();
}

} // namespace mylang
