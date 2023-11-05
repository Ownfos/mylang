#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

// TODO: accept CompoundStmt when ready.
FuncDecl::FuncDecl(bool should_export, const Token& name, const std::optional<Token>& return_type, const std::vector<Parameter>& parameters/*, std::shared_ptr<CompoundStmt> body*/)
    : GlobalDecl(should_export, name)
    , m_return_type(return_type)
    , m_parameters(parameters)
{}

void FuncDecl::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const std::optional<Token>& FuncDecl::ReturnType() const
{
    return m_return_type;
}

const std::vector<Parameter>& FuncDecl::Parameters() const
{
    return m_parameters;
}

} // namespace mylang
