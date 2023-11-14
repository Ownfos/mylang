#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

FuncDecl::FuncDecl(bool should_export, const Token& name, std::optional<Type> return_type, const std::vector<Parameter>& parameters, std::shared_ptr<Stmt> body)
    : GlobalDecl(should_export, name)
    , m_return_type(return_type)
    , m_parameters(parameters)
    , m_body(body)
{}

void FuncDecl::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
    visitor->IncreaseDepth();
    m_body->Accept(visitor);
    visitor->DecreaseDepth();
}

const std::optional<Type>& FuncDecl::ReturnType() const
{
    return m_return_type;
}

const std::vector<Parameter>& FuncDecl::Parameters() const
{
    return m_parameters;
}

} // namespace mylang
