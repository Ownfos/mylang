#include "parser/ast/visitor/TypeChecker.h"
#include "parser/ast/Module.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"

#include "parser/ast/stmt/CompoundStmt.h"
#include "parser/ast/stmt/IfStmt.h"
#include "parser/ast/stmt/ForStmt.h"
#include "parser/ast/stmt/WhileStmt.h"
#include "parser/ast/stmt/JumpStmt.h"
#include "parser/ast/stmt/VarDeclStmt.h"
#include "parser/ast/stmt/ExprStmt.h"

#include "parser/ast/varinit/VarInitExpr.h"
#include "parser/ast/varinit/VarInitList.h"

#include "parser/ast/expr/ArrayAccessExpr.h"
#include "parser/ast/expr/BinaryExpr.h"
#include "parser/ast/expr/FuncCallExpr.h"
#include "parser/ast/expr/Identifier.h"
#include "parser/ast/expr/Literal.h"
#include "parser/ast/expr/MemberAccessExpr.h"
#include "parser/ast/expr/PostfixExpr.h"
#include "parser/ast/expr/PrefixExpr.h"

#include "parser/type/base/StructType.h"
#include "parser/type/base/FuncType.h"

#include "parser/SemanticError.h"

#include <format>

namespace mylang
{

TypeChecker::TypeChecker(ProgramEnvironment& environment)
    : m_environment(environment)
{}

void TypeChecker::PreorderVisit(Module* node)
{
    m_context_module_name = node->ModuleName().lexeme;
}

void TypeChecker::PreorderVisit(FuncDecl* node)
{
    // Record that all of the following statements are
    // executed within this function's context.
    //
    // This function signature will be used to validate return statement.
    m_current_function = node;
}

void TypeChecker::PreorderVisit(StructDecl* node)
{
    // Make sure that all member variables with struct type are valid.
    for (const auto& member : node->Members())
    {
        if (!member.type.IsValid(m_environment, m_context_module_name))
        {
            auto message = std::format("member variable \"{}\" of struct \"{}\" tried to use invalid type \"{}\"",
                member.name.lexeme,
                node->Name().lexeme,
                member.type.ToString()
            );
            throw SemanticError(member.name.start_pos, message);
        }
    }
}

void TypeChecker::PostorderVisit(CompoundStmt* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(IfStmt* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(ForStmt* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(WhileStmt* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(JumpStmt* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(VarDeclStmt* node)
{
    auto var_type = node->DeclType();
    auto initializer_type = GetNodeType(node->Initializer());

    // TODO: allow type coersion or initializer list of smaller dimensions
    //
    // example)
    // f32 <- i32 (valid)
    // i32[100] <- i32[2] (valid)
    //
    // Note: array size should match on assignment, but initializers do not require strict size match!
    if (var_type != initializer_type)
    {
        auto message = std::format("trying to assign expression of type \"{}\" to \"{}\" type variable",
            initializer_type.ToString(),
            var_type.ToString()
        );
        throw SemanticError(node->Name().start_pos, message);
    }
}

void TypeChecker::PostorderVisit(ExprStmt* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(VarInitExpr* node)
{
    // VarInitExpr has same type as its internal Expr node.
    SetNodeType(node, GetNodeType(node->Expression()));
}

void TypeChecker::PostorderVisit(VarInitList* node)
{
    // TODO: construct array type from elements in the list.
    // case 1) base type and array size match => good!
    // case 2) base type matches but array size differs => use the maximum size to afford all of them
    // case 3) base type doesn't match => throw an exception
}

void TypeChecker::PostorderVisit(ArrayAccessExpr* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(BinaryExpr* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(FuncCallExpr* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(Identifier* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(Literal* node)
{
    SetNodeType(node, node->DeclType());
}

void TypeChecker::PostorderVisit(MemberAccessExpr* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(PostfixExpr* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(PrefixExpr* node)
{
    // TODO: implement
}

void TypeChecker::SetNodeType(const IAbstractSyntaxTree* node, const Type& type)
{
    m_type_dict.insert({node, type});
}

const Type& TypeChecker::GetNodeType(const IAbstractSyntaxTree* node) const
{
    return m_type_dict.at(node);
}

} // namespace mylang
