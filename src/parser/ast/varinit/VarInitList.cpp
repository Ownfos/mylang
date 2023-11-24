#include "parser/ast/varinit/VarInitList.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

VarInitList::VarInitList(const std::vector<std::shared_ptr<VarInit>>& initializer_list)
    : m_initializer_list(initializer_list)
{}

void VarInitList::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    for (auto& initializer : m_initializer_list)
    {
        initializer->Accept(visitor);
    }
    visitor->PostorderVisit(this);
}

const std::vector<std::shared_ptr<VarInit>>& VarInitList::InitializerList() const
{
    return m_initializer_list;
}

} // namespace mylang
