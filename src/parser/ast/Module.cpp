#include "parser/ast/Module.h"
#include "parser/ast/globdecl/GlobalDecl.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

Module::Module(const Token& module_name, const std::vector<ModuleImportInfo>& import_list, const std::vector<std::shared_ptr<GlobalDecl>>& global_declarations)
    : m_module_name(module_name)
    , m_import_list(import_list)
    , m_global_declarations(global_declarations)
{}

void Module::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
    
    visitor->IncreaseDepth();
    for (auto& decl : m_global_declarations)
    {
        decl->Accept(visitor);
    }
    visitor->DecreaseDepth();
}

const Token& Module::ModuleName() const
{
    return m_module_name;
}

const std::vector<ModuleImportInfo>& Module::ImportList() const
{
    return m_import_list;
}

} // namespace mylang
