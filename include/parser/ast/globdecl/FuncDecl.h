#ifndef MYLANG_FUNC_DECL_H
#define MYLANG_FUNC_DECL_H

#include "parser/ast/globdecl/GlobalDecl.h"
#include "parser/ast/globdecl/Parameter.h"
#include "parser/ast/stmt/Stmt.h"
#include <vector>
#include <optional>

namespace mylang
{

class FuncDecl : public GlobalDecl
{
public:
    // 'should_export' is true iff "export" prefix was found.
    // 'return_Type' can be empty if void is intended.
    FuncDecl(bool should_export, const Token& name, std::optional<Type> return_type, const std::vector<std::shared_ptr<Parameter>>& parameters, std::shared_ptr<Stmt> body);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;

    virtual const Token& Name() const override;
    virtual const Type& DeclType() const override;
    virtual bool ShouldExport() const override;

    const Type& ReturnType() const;
    const std::vector<std::shared_ptr<Parameter>>& Parameters() const;
    Stmt* Body();

private:
    bool m_should_export;
    Token m_name;
    Type m_return_type;
    std::vector<std::shared_ptr<Parameter>> m_parameters;
    std::shared_ptr<Stmt> m_body;

    // The type of functions itself.
    Type m_type;
};

} // namespace mylang

#endif // MYLANG_FUNC_DECL_H
