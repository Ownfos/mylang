#ifndef MYLANG_FUNC_DECL_H
#define MYLANG_FUNC_DECL_H

#include "parser/ast/globdecl/GlobalDecl.h"
#include "parser/ast/stmt/Stmt.h"
#include "parser/type/base/FuncType.h"
#include <vector>
#include <optional>

namespace mylang
{

struct Parameter
{
    Token name;
    ParamType type;
};

class FuncDecl : public GlobalDecl
{
public:
    // 'should_export' is true iff "export" prefix was found.
    // 'return_Type' can be empty if void is intended.
    FuncDecl(bool should_export, const Token& name, std::optional<Type> return_type, const std::vector<Parameter>& parameters, std::shared_ptr<Stmt> body);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

    virtual const Token& Name() const override;
    virtual const Type& DeclType() const override;
    virtual bool ShouldExport() const override;

    const std::optional<Type>& ReturnType() const;
    const std::vector<Parameter>& Parameters() const;

private:
    bool m_should_export;
    Token m_name;
    std::optional<Type> m_return_type;
    std::vector<Parameter> m_parameters;
    std::shared_ptr<Stmt> m_body;

    // The type of functions itself.
    Type m_type;
};

} // namespace mylang

#endif // MYLANG_FUNC_DECL_H
