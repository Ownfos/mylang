#include "parser/ProgramEnvironment.h"

namespace mylang
{

void ProgramEnvironment::AddModuleDeclaration(const Module* module)
{
    auto module_name = std::string_view(module->ModuleName().lexeme);

    // If this was the first module implementation file,
    // create a new entry for module information.
    if (m_modules.find(module_name) == m_modules.end())
    {
        m_modules.insert({module_name, {}});
    }

    // Collect module import directives.
    // It is okay, until now, to have duplicate entries.
    auto& module_info = GetModuleInfo(module_name);
    for (const auto& import_info : module->ImportList())
    {
        module_info.import_list.insert(import_info);
    }
}

void ProgramEnvironment::OpenScope(std::string_view context_module_name)
{
    auto& module_info = GetModuleInfo(context_module_name);
    module_info.local_symbol_table.OpenScope();
}

void ProgramEnvironment::CloseScope(std::string_view context_module_name)
{
    auto& module_info = GetModuleInfo(context_module_name);
    module_info.local_symbol_table.CloseScope();
}

void ProgramEnvironment::AddSymbol(
    std::string_view context_module_name,
    Decl* declaration,
    bool is_public
)
{
    auto& module_info = GetModuleInfo(context_module_name);
    module_info.local_symbol_table.AddSymbol(declaration, is_public);
}

Symbol ProgramEnvironment::FindSymbol(
    std::string_view context_module_name,
    std::string_view symbol_name
) const
{
    auto& module_info = GetModuleInfo(context_module_name);

    // Look for local symbol.
    if (auto symbol = module_info.local_symbol_table.FindSymbol(symbol_name))
    {
        return symbol.value();
    }
    // If we failed to find one, look for public symbols in the imported modules.
    else
    {
        // This variable is used to prevent infinite search loop
        // when two modules have circular dependency.
        auto visited_modules = std::set<std::string_view>{context_module_name};

        // Recursively search for a public symbol with the specified name.
        for (const auto& import_info : module_info.import_list)
        {
            if (auto symbol = FindImportedSymbol(import_info.name.lexeme, symbol_name, visited_modules))
            {
                return symbol.value();
            }
        }

        // The symbol we want to find doesn't exist!
        // TODO: replace std::exception with SemanticError class
        throw std::exception("trying to find a symbol that doesn't exist");
    }
}

std::optional<Symbol> ProgramEnvironment::FindImportedSymbol(
    std::string_view context_module_name,
    std::string_view symbol_name,
    std::set<std::string_view>& visited_modules
) const
{
    // Do not search for module symbols more than once.
    if (visited_modules.count(context_module_name) > 0) return {};

    auto& module_info = GetModuleInfo(context_module_name);

    // Look for local public symbol.
    auto symbol = module_info.local_symbol_table.FindSymbol(symbol_name);
    if (symbol.has_value() && symbol->is_public)
    {
        return symbol.value();
    }
    else
    {
        visited_modules.insert(context_module_name);
        
        // Recursively search for a public symbol with the specified name.
        for (const auto& import_info : module_info.import_list)
        {
            // Do NOT propagate module dependency for private import directives!
            if (!import_info.should_export) continue;

            if (auto symbol = FindImportedSymbol(import_info.name.lexeme, symbol_name, visited_modules))
            {
                return symbol.value();
            }
        }

        // Failed to find public symbol declaration in this module's context.
        return {};
    }
}

ModuleInfo& ProgramEnvironment::GetModuleInfo(std::string_view name)
{
    return m_modules.at(name);
}

const ModuleInfo& ProgramEnvironment::GetModuleInfo(std::string_view name) const
{
    return m_modules.at(name);
}

} // namespace mylang
