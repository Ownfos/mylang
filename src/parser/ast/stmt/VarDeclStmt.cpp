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
    visitor->Visit(this);
}

const SourcePos& VarDeclStmt::StartPos() const
{
    return m_name.start_pos;
}

const Token& VarDeclStmt::Name() const
{
    return m_name;
}

const Type& VarDeclStmt::DeclType() const
{
    return m_type;
}

VarInit* VarDeclStmt::Initializer()
{
    return m_initializer.get();
}

} // namespace mylang
