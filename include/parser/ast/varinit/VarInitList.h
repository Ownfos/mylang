#ifndef MYLANG_VAR_INIT_LIST_H
#define MYLANG_VAR_INIT_LIST_H

#include "parser/ast/varinit/VarInit.h"
#include <vector>
#include <memory>

namespace mylang
{

class VarInitList : public VarInit
{
public:
    VarInitList(const std::vector<std::shared_ptr<VarInit>>& initializer_list);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;

    const std::vector<std::shared_ptr<VarInit>>& InitializerList() const;

private:
    std::vector<std::shared_ptr<VarInit>> m_initializer_list;
};

} // namespace mylang

#endif // MYLANG_VAR_INIT_LIST_H
