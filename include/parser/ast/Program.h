#ifndef MYLANG_PROGRAM_H
#define MYLANG_PROGRAM_H

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
};

class Program : public IAbstractSyntaxTree
{
public:
    Program(const Token& module_name, const std::vector<ModuleImportInfo>& import_list, const std::vector<std::shared_ptr<GlobalDecl>>& global_declarations);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

    const Token& ModuleName() const;
    const std::vector<ModuleImportInfo>& ImportList() const;

private:
    Token m_module_name;
    std::vector<ModuleImportInfo> m_import_list;
    std::vector<std::shared_ptr<GlobalDecl>> m_global_declarations;
};

} // namespace mylang

#endif // MYLANG_PROGRAM_H