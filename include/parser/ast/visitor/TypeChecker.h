#ifndef MYLANG_TYPE_CHECKER_H
#define MYLANG_TYPE_CHECKER_H

#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include "parser/ProgramEnvironment.h"
#include <map>
#include <stack>

namespace mylang
{

class StructType;
class FuncType;
class IAbstractSyntaxTree;

// Checks the following type contraints:
// 1. array size should be a positive integer (0 is not allowed)
// 2. referenced variable should be declared before use
// 3. struct type should be declared before use
// 4. rvalue expression is not allowed as an out/inout parameter
// 5. condition expression should have bool type
// 6. function call arguments should match the declaration
// 7. member access should specify a valid member variable name
// 8. array access should not happen on non-array type (e.g. i[0] where i is an int)
// 9. array access index expression should have int type
// 10. return value's type should match function definition
class TypeChecker : public IAbstractSyntaxTreeVisitor
{
public:
    TypeChecker(ProgramEnvironment& environment);

    virtual void PreorderVisit(Module* node) override;
    virtual void PreorderVisit(FuncDecl* node) override;
    virtual void PreorderVisit(StructDecl* node) override;

    virtual void PostorderVisit(CompoundStmt* node) override;

    // TODO: verify that condition expr has bool type
    virtual void PostorderVisit(IfStmt* node) override;

    // TODO: verify that condition expr has bool type
    virtual void PostorderVisit(ForStmt* node) override;
    
    // TODO: verify that condition expr has bool type
    virtual void PostorderVisit(WhileStmt* node) override;

    // TODO: verify that return expr matches the function signature
    virtual void PostorderVisit(JumpStmt* node) override;

    // TODO: verify that varinit type matches decltype
    virtual void PostorderVisit(VarDeclStmt* node) override;
    virtual void PostorderVisit(ExprStmt* node) override;

    virtual void PostorderVisit(VarInitExpr* node) override;

    // TODO: construct a minimal array type that a variable should have
    virtual void PostorderVisit(VarInitList* node) override;

    // TODO: verify that lhs operand is an array type and the index expr is int
    virtual void PostorderVisit(ArrayAccessExpr* node) override;

    virtual void PostorderVisit(BinaryExpr* node) override;

    // TODO: verify that arguments match the function signature
    virtual void PostorderVisit(FuncCallExpr* node) override;

    virtual void PostorderVisit(Identifier* node) override;
    virtual void PostorderVisit(Literal* node) override;

    // TODO: verify that lhs operand is a struct type variable and rhs is a valid member name.
    virtual void PostorderVisit(MemberAccessExpr* node) override;

    virtual void PostorderVisit(PostfixExpr* node) override;
    virtual void PostorderVisit(PrefixExpr* node) override;

private:
    void ValidateType(const Type& type);
    void ValidateStructType(const StructType* type);
    void ValidateFuncType(const FuncType* type);

    ProgramEnvironment& m_environment;

    // The name of module we are parsing
    std::string_view m_context_module_name;

    // Stores temporary expression node's type
    std::map<IAbstractSyntaxTree*, Type> m_type_dict;

    // Used to store a currently analyzed function's signature.
    // Return type matching is the key purpose for saving this info.
    FuncDecl* m_current_function;
};

} // namespace mylang

#endif // MYLANG_TYPE_CHECKER_H
