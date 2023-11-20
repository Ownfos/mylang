#include "parser/ast/visitor/GlobalSymbolScanner.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"

namespace mylang
{

GlobalSymbolScanner::GlobalSymbolScanner(ProgramEnvironment& environment)
    : m_environment(environment)
{}

void GlobalSymbolScanner::PreorderVisit(Module* node)
{
    m_module_name = node->ModuleName().lexeme;
    m_environment.AddModuleDeclaration(node);
}

void GlobalSymbolScanner::PreorderVisit(FuncDecl* node)
{
    m_environment.AddSymbol(m_module_name, node, node->ShouldExport());
}

void GlobalSymbolScanner::PreorderVisit(StructDecl* node)
{
    m_environment.AddSymbol(m_module_name, node, node->ShouldExport());
}

} // namespace mylang
