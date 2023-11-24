#ifndef MYLANG_PARAMETER_H
#define MYLANG_PARAMETER_H

#include "parser/ast/Decl.h"
#include "parser/type/base/FuncType.h"

namespace mylang
{

class Parameter : public Decl
{
public:
    Parameter(const Token& name, const ParamType& param_type);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

    virtual const Token& Name() const override;
    virtual const Type& DeclType() const override;

    const ParamType& DeclParamType() const;

private:
    Token m_name;
    ParamType m_param_type;
};

} // namespace mylang

#endif // MYLANG_PARAMETER_H
