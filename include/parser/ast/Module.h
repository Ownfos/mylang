#ifndef MYLANG_MODULE_H
#define MYLANG_MODULE_H

#include "parser/ast/IAbstractSyntaxTree.h"
#include "lexer/Token.h"
#include <vector>
#include <memory>

namespace mylang
{

class GlobalDecl;

struct ModuleImportInfo
{
    bool should_export;
    Token name;

    // Comparator that enables use of std::set<ModuleImportInfo>.
    // This simply compares the name token's lexeme in lexicographical order.
    bool operator<(const ModuleImportInfo& other) const;
};

// Module represents an implementation file for a module.
// A single logical module can be implemented with multiple source files,
// so a Module node can be thought of as a paritial implementation or a module fragment.
class Module : public IAbstractSyntaxTree
{
public:
    Module(const Token& module_name, const std::vector<ModuleImportInfo>& import_list, const std::vector<std::shared_ptr<GlobalDecl>>& global_declarations);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

    const Token& ModuleName() const;
    const std::vector<ModuleImportInfo>& ImportList() const;

private:
    Token m_module_name;
    std::vector<ModuleImportInfo> m_import_list;
    std::vector<std::shared_ptr<GlobalDecl>> m_global_declarations;
};

} // namespace mylang

#endif // MYLANG_MODULE_H
