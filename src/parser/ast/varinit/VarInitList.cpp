#include "parser/ast/varinit/VarInitList.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

VarInitList::VarInitList(const std::vector<std::shared_ptr<VarInit>>& initializer_list)
    : m_initializer_list(initializer_list)
{}

void VarInitList::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& VarInitList::StartPos() const
{
    return m_initializer_list.front()->StartPos();
}

const std::vector<std::shared_ptr<VarInit>>& VarInitList::InitializerList() const
{
    return m_initializer_list;
}

} // namespace mylang
