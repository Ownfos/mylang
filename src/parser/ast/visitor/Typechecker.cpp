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

void TypeChecker::PreorderVisit(VarDeclStmt* node)
{
    // This information will be used to report source location
    // when initializer list generates a semantic error.
    m_latest_source_pos = node->Name().start_pos;
}

void TypeChecker::PostorderVisit(VarDeclStmt* node)
{
    // We will check if the initializer type is appropriate for this variable.
    auto var_type = node->DeclType();
    auto init_type = GetNodeType(node->Initializer());

    // Check if base type is compatible.
    if (var_type.BaseType()->ToString() != init_type.BaseType()->ToString())
    {
        // TODO: change the condition so that we throw exception
        //       only when type coercion is impossible.
        //       ex) f32 <- i32 (valid)
        //           i32 <- f32 (invalid)
        if (true)
        {
            auto message = std::format("trying to assign expression of type \"{}\" to \"{}\" type variable",
                init_type.ToString(),
                var_type.ToString()
            );
            throw SemanticError(node->Name().start_pos, message);
        }
    }

    // Check if initializer list has same number of dimensions.
    // ex) arr: i32[100] = {{1}, {2}} (invalid: dimension mismatch)
    auto var_array_sizes = var_type.ArraySize();
    auto init_array_sizes = init_type.ArraySize();
    if (var_array_sizes.size() != init_array_sizes.size())
    {
        auto message = std::format("trying to assign expression of type \"{}\" to \"{}\" type variable",
            init_type.ToString(),
            var_type.ToString()
        );
        throw SemanticError(node->Name().start_pos, message);
    }

    // Check if initializer list has less-or-equal size compared to the variable type.
    // ex) arr: i32[100] = {0}; (valid: partial initialization)
    for (int i=0;i<var_array_sizes.size();++i)
    {
        if (var_array_sizes[i] < init_array_sizes[i])
        {
            auto message = std::format("array size of initializer \"\" exceeds variable's type \"{}\"",
                init_type.ToString(),
                var_type.ToString()
            );
            throw SemanticError(node->Name().start_pos, message);
        }
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


// We will find the minimum array size which can
// accommodate all elements in this initializer list.
//
// Starting from the type of the first element,
// the array size on each dimension will grow whenever
// a sibling element has larger array size on that dimension.
//
// example)
// Given list is {{1}, {2, 3}, {4}}.
//
// Start with i32[1]
// => to accommodate {2, 3}, increase size to i32[2]
// => to accommodate {4}, we don't need any extra work.
//
// After iteration is done, append extra dimension on the left
// to note that this is an array of elements
// => i32[3][2]
void TypeChecker::PostorderVisit(VarInitList* node)
{
    auto list_type = GetNodeType(node->InitializerList().begin()->get());

    for (const std::shared_ptr<VarInit>& elem : node->InitializerList())
    {
        auto elem_type = GetNodeType(elem.get());

        // Do not allow mixing types inside a single initializer list.
        auto expected_base_type_name = list_type.BaseType()->ToString();
        auto elem_base_type_name = elem_type.BaseType()->ToString();
        if (expected_base_type_name != elem_base_type_name)
        {
            auto message = std::format("trying to mix types \"{}\" and \"{}\" inside an initializer list",
                expected_base_type_name,
                elem_base_type_name
            );
            throw SemanticError(m_latest_source_pos, message);
        }

        // Update list type to a bigger array which can store all elements in this list.
        // When number of dimensions differ between elements, an exception will be thrown.
        try
        {
            list_type.MergeArrayDim(elem_type);
        }
        catch(const std::exception&)
        {
            throw SemanticError(m_latest_source_pos, "every element in an initializer list should have same dimension");
        }
    }

    list_type.AddLeftmostArrayDim(static_cast<int>(node->InitializerList().size()));

    SetNodeType(node, list_type);
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
