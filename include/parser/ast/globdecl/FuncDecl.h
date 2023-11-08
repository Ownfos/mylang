#ifndef MYLANG_FUNC_DECL_H
#define MYLANG_FUNC_DECL_H

#include "parser/ast/globdecl/GlobalDecl.h"
#include "parser/ast/stmt/CompoundStmt.h"
#include "parser/type/Type.h"
#include <vector>
#include <optional>

namespace mylang
{

struct Parameter
{
    Token name;
    std::optional<Token> usage;
    std::shared_ptr<Type> type;
};

class FuncDecl : public GlobalDecl
{
public:
    FuncDecl(bool should_export, const Token& name, std::shared_ptr<Type> return_type, const std::vector<Parameter>& parameters, std::shared_ptr<CompoundStmt> body);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

    const Type* ReturnType() const;
    const std::vector<Parameter>& Parameters() const;

private:
    std::shared_ptr<Type> m_return_type;
    std::vector<Parameter> m_parameters;
    std::shared_ptr<CompoundStmt> m_body;
};

} // namespace mylang

#endif // MYLANG_FUNC_DECL_H
