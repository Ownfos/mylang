#ifndef MYLANG_PROGRAM_ENVIRONMENT_H
#define MYLANG_PROGRAM_ENVIRONMENT_H

#include "parser/ast/Module.h"
#include "parser/SymbolTable.h"
#include <map>
#include <set>

namespace mylang
{

// ModuleInfo is used to aggregate declarations and import directives for a logical module.
//
// Note that a ModuleInfo instance corresponds to a single logical module,
// whereas a single logical module can be implemented across multiple source files.
//
// example)
// file1: "module a; import math; ..."
// file2: "module a; ..."
// file3: "module b; ..."
// file4: "module c; ..."
struct ModuleInfo
{
    std::set<ModuleImportInfo> import_list;
    SymbolTable local_symbol_table;
};

// Manages symbol table for each module and provide
// symbol searching functionality across imported modules.
class ProgramEnvironment
{
public:
    // Record the module name and module import directives.
    // In case of multiple files implementing a single logical module,
    // the first AddModuleDeclaration() will create a new symbol table
    // and the follosing invocations will simply append import directives.
    void AddModuleDeclaration(const Module* module);

    // Wrapper functions for SymbolTable::OpenScope() and CloseScope().
    void OpenScope(std::string_view context_module_name);
    void CloseScope(std::string_view context_module_name);

    // Wrapper function for SymbolTable::AddSymbol().
    void AddSymbol(
        std::string_view context_module_name,
        Decl* declaration,
        bool is_public
    );

    // Try to find a symbol from a given module context by following order:
    // 1. module's local symbol table (private or public)
    // 2. imported symbols from other modules (public)
    //
    // If we fail to find a symbol, an exception will be thrown.
    Symbol FindSymbol(
        std::string_view context_module_name,
        std::string_view symbol_name
    ) const;

private:
    // Returns a symbol declared with "export" keyword,
    // including the ones imported from other modules.
    // 
    // The cache variable 'visited_modules' is used
    // to avoid circular dependency among multiple modules.
    //
    // example)
    // A imports B and C, where C defines a function "foo".
    // If we want to find a symbol named "foo",
    // the function call stack will look something like this:
    //
    // FindSymbol("A", "foo")
    // -> look for any "foo" from local symbol table of A (fail)
    // -> FindImportedSymbol("B", "foo", {"A"})
    //     -> look for public "foo" from local symbol table of B (fail)
    // -> FindImportedSymbol("C", "foo", {"A", "B"})
    //     -> look for public "foo" from local symbol table of C (success)
    //     -> return Symbol instance for "foo"
    // -> return Symbol instance for "foo"
    std::optional<Symbol> FindImportedSymbol(
        std::string_view context_module_name,
        std::string_view symbol_name,
        std::set<std::string_view>& visited_modules
    ) const;

    // Returns a ModuleInfo instance for a module with specified name.
    // The const version is used in FindSymbol().
    ModuleInfo& GetModuleInfo(std::string_view name);
    const ModuleInfo& GetModuleInfo(std::string_view name) const;

    // Maps a module name to its corresponding ModuleInfo instance.
    std::map<std::string_view, ModuleInfo> m_modules;
};

} // namespace mylang

#endif // MYLANG_PROGRAM_ENVIRONMENT_H
