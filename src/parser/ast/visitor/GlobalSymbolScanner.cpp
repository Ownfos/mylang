#include "parser/ast/visitor/GlobalSymbolScanner.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"

namespace mylang
{

GlobalSymbolScanner::GlobalSymbolScanner(ProgramEnvironment& environment)
    : m_environment(environment)
{}

void GlobalSymbolScanner::Visit(Module* node)
{
    m_module_name = node->ModuleName().lexeme;
    m_environment.AddModuleDeclaration(node);

    for (const auto& decl : node->Declarations())
    {
        decl->Accept(this);
    }
}

void GlobalSymbolScanner::Visit(FuncDecl* node)
{
    m_environment.AddSymbol(m_module_name, node, node->ShouldExport());
}

void GlobalSymbolScanner::Visit(StructDecl* node)
{
    m_environment.AddSymbol(m_module_name, node, node->ShouldExport());
}

} // namespace mylang
