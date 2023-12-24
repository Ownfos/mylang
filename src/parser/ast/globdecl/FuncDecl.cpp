#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <ranges>

namespace mylang
{

std::shared_ptr<FuncType> ConstructFuncType(const Type& return_type, const std::vector<std::shared_ptr<Parameter>>& parameters)
{
    // Callback function that extracts Parameter::type
    auto extract_type = [](const std::shared_ptr<Parameter>& param){
        return param->DeclParamType();
    };

    // Create a vector of ParamType
    auto types = parameters | std::views::transform(extract_type);
    auto param_types = std::vector<ParamType>{types.begin(), types.end()};

    return std::make_shared<FuncType>(param_types, return_type);
}

FuncDecl::FuncDecl(bool should_export, const Token& name, std::optional<Type> return_type, const std::vector<std::shared_ptr<Parameter>>& parameters, std::shared_ptr<Stmt> body)
    : m_should_export(should_export)
    , m_name(name)
    , m_return_type(return_type.value_or(CreateVoidType()))
    , m_parameters(parameters)
    , m_body(body)
    , m_type(ConstructFuncType(m_return_type, parameters))
{}

void FuncDecl::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& FuncDecl::StartPos() const
{
    return m_name.start_pos;
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

const Type& FuncDecl::ReturnType() const
{
    return m_return_type;
}

const std::vector<std::shared_ptr<Parameter>>& FuncDecl::Parameters() const
{
    return m_parameters;
}

} // namespace mylang
