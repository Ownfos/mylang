#ifndef MYLANG_SYMBOL_TABLE_H
#define MYLANG_SYMBOL_TABLE_H

#include "parser/ast/Decl.h"
#include <memory>
#include <vector>
#include <optional>

namespace mylang
{

struct Symbol
{
    Decl* declaration;
    bool is_public;
    int scope_level;
};

class SymbolTable
{
public:
    // Increases scope level.
    void OpenScope();

    // Decreases scope level and remove all symbols within the highest scope.
    void CloseScope();

    // Inserts a new entry to the table.
    // Throws an exception when redefinition of a symbol on same scope happens.
    void AddSymbol(Decl* declaration, bool is_public);

    // Returns a symbol entry with the specified name.
    // When there are several entries with identical name,
    // the one with higher scope level will be selected.
    // If there were no matches, an empty optional will be returned.
    std::optional<Symbol> FindSymbol(std::string_view name) const;

    // Returns all symbols declared with "export" keyword.
    // These symbols are visible on other modules which import this module.
    std::vector<Symbol> GlobalSymbols() const;

    // Returns all symbols declared without "export" keyword.
    // These symbols are visible only inside this module.
    std::vector<Symbol> LocalSymbols() const;

private:
    // Scope level increase/decrease whenever
    // OpenScope() or CloseScope() is invoked.
    // Level 0 corresponds to the global scope of a module.
    int m_current_scope_level = 0;

    // A stack-based data structure where TOS comes at the back.
    std::vector<Symbol> m_symbols;
};

} // namespace mylang

#endif // MYLANG_SYMBOL_TABLE_H
