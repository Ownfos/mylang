#ifndef MYLANG_TYPE_CHECKER_H
#define MYLANG_TYPE_CHECKER_H

#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include "parser/ProgramEnvironment.h"
#include <map>
#include <stack>

namespace mylang
{

class Expr;
class StructType;
class FuncType;
class IAbstractSyntaxTree;
struct ParamType;

struct ExprTrait
{
    bool is_lvalue;
    Type type;
};

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

    virtual void Visit(Module* node) override;

    virtual void Visit(FuncDecl* node) override;
    virtual void Visit(Parameter* node) override;

    virtual void Visit(StructDecl* node) override;

    virtual void Visit(CompoundStmt* node) override;

    virtual void Visit(IfStmt* node) override;

    virtual void Visit(ForStmt* node) override;
    
    virtual void Visit(WhileStmt* node) override;

    virtual void Visit(JumpStmt* node) override;

    virtual void Visit(VarDeclStmt* node) override;

    virtual void Visit(VarInitExpr* node) override;

    virtual void Visit(VarInitList* node) override;

    virtual void Visit(ArrayAccessExpr* node) override;

    virtual void Visit(BinaryExpr* node) override;

    virtual void Visit(FuncCallExpr* node) override;

    virtual void Visit(Identifier* node) override;
    virtual void Visit(Literal* node) override;


    virtual void Visit(MemberAccessExpr* node) override;

    virtual void Visit(PostfixExpr* node) override;
    virtual void Visit(PrefixExpr* node) override;

private:
    // Getter and setter that wraps std::map implementation details
    void SetExprTrait(const IAbstractSyntaxTree* node, const Type& type, bool is_lvalue = false);
    const ExprTrait& GetExprTrait(const IAbstractSyntaxTree* node) const;

    // Checks if a type is valid (i.e. all struct types are visible in this context).
    // If not, semantic error will be thrown.
    void ValidateTypeExistence(const Type& type, std::string_view who, const SourcePos& where);

    // Check if the expression has bool type.
    // If not, semantic error will be thrown.
    void ValidateConditionExprType(const Expr* condition_expr);

    // Check if the expression is an lvalue.
    // If not, semantic error will be thrown.
    void ValidateLValueQualifier(const Expr* expr, std::string_view who, const SourcePos& where);

    // Find the declaration of the type.
    // If it wasn't a struct type, semantic error will be thrown.
    const StructDecl* TryToFindStructTypeDecl(const Type &type, const SourcePos &where);

    ProgramEnvironment& m_environment;

    // The name of module we are parsing
    std::string_view m_context_module_name;

    // Stores temporary expression node's type
    std::map<const IAbstractSyntaxTree*, ExprTrait> m_expr_trait_dict;

    // Used to store a currently analyzed function's signature.
    // Return type matching is the key purpose for saving this info.
    FuncDecl* m_current_function;
};

} // namespace mylang

#endif // MYLANG_TYPE_CHECKER_H
