#include "parser/SymbolTable.h"

namespace mylang
{

void SymbolTable::OpenScope()
{
    ++m_current_scope_level;
}

void SymbolTable::CloseScope()
{
    --m_current_scope_level;

    // Delete all symbols that went out of scope
    auto is_out_of_scope = [=](const Symbol& symbol) {
        return symbol.scope_level > m_current_scope_level;
    };
    while (!m_symbols.empty() && is_out_of_scope(m_symbols.back()))
    {
        m_symbols.pop_back();
    }
}

void SymbolTable::AddSymbol(Decl* declaration, bool is_public)
{
    // Check if we are violating ODR.
    auto same_name = FindSymbol(declaration->Name().lexeme);
    if (same_name.has_value() && same_name->scope_level == m_current_scope_level)
    {
        // TODO: throw ODR violation error.
    }

    m_symbols.emplace_back(declaration, is_public, m_current_scope_level);
}

std::optional<Symbol> SymbolTable::FindSymbol(std::string_view name) const
{
    // Search for the symbol with matching name and the highest scope level.
    for (auto it = m_symbols.rbegin(); it != m_symbols.rend(); ++it)
    {
        if (it->declaration->Name().lexeme == name)
        {
            return *it;
        }
    }

    // Found nothing...
    return {};
}

} // namespace mylang
