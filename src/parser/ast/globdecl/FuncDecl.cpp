#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <ranges>

namespace mylang
{

std::shared_ptr<FuncType> ConstructFuncType(std::optional<Type> return_type, const std::vector<Parameter>& parameters)
{
    // Callback function that extracts Parameter::type
    auto extract_type = [](const Parameter& param){
        return param.type;
    };

    // Create a vector of ParamType
    auto types = parameters | std::views::transform(extract_type);
    auto param_types = std::vector<ParamType>{types.begin(), types.end()};

    return std::make_shared<FuncType>(param_types, return_type);
}

FuncDecl::FuncDecl(bool should_export, const Token& name, std::optional<Type> return_type, const std::vector<Parameter>& parameters, std::shared_ptr<Stmt> body)
    : m_should_export(should_export)
    , m_name(name)
    , m_return_type(return_type)
    , m_parameters(parameters)
    , m_body(body)
    , m_type(ConstructFuncType(return_type, parameters))
{}

void FuncDecl::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    m_body->Accept(visitor);
    visitor->PostorderVisit(this);
}

const Token& FuncDecl::Name() const
{
    return m_name;
}

const Type& FuncDecl::DeclType() const
{
    return m_type;
}

bool FuncDecl::ShouldExport() const
{
    return m_should_export;
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
