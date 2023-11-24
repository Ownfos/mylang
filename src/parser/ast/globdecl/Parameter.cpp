#include "parser/ast/globdecl/Parameter.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

Parameter::Parameter(const Token& name, const ParamType& param_type)
    : m_name(name), m_param_type(param_type)
{}

void Parameter::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    visitor->PostorderVisit(this);
}

const Token& Parameter::Name() const
{
    return m_name;
}

const Type& Parameter::DeclType() const
{
    return m_param_type.type;
}

const ParamType& Parameter::DeclParamType() const
{
    return m_param_type;
}

} // namespace mylang
