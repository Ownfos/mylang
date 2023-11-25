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

#include "parser/type/base/PrimitiveType.h"
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

    // Check if the return type is valid.
    // Note: parameter types will be validated on child nodes.
    auto ret_type = node->ReturnType();
    if (ret_type.has_value() && !ret_type->IsValid(m_environment, m_context_module_name))
    {
        auto message = std::format("function \"{}\" tried to use invalid return type \"{}\"",
            node->Name().lexeme,
            ret_type->ToString()
        );
        throw SemanticError(node->StartPos(), message);
    }

    m_environment.OpenScope(m_context_module_name);
}

void TypeChecker::PostorderVisit(FuncDecl* node)
{
    m_environment.CloseScope(m_context_module_name);
}

void TypeChecker::PreorderVisit(Parameter* node)
{
    // Throw an error if the type is invalid in this module's context.
    const auto param_type = node->DeclType();
    if (!param_type.IsValid(m_environment, m_context_module_name))
    {
        auto message = std::format("parameter \"{}\" tried to use invalid type \"{}\"",
            node->Name().lexeme,
            param_type.ToString()
        );
        throw SemanticError(node->StartPos(), message);
    }

    // If the type was valid, add it to the local symbol table.
    // Note: corresponding FuncDecl opens a new scope,
    //       so the parameters are added to the nested scope (not the global one!).
    m_environment.AddSymbol(m_context_module_name, node, false);
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

void TypeChecker::ValidateConditionExprType(const Expr* condition_expr)
{
    auto type_name = GetNodeType(condition_expr).ToString();
    if (type_name != "bool")
    {
        auto message = std::format("condition expression should have bool type, instead of \"{}\"",
            type_name
        );
        throw SemanticError(condition_expr->StartPos(), message);
    }
}

void TypeChecker::PostorderVisit(IfStmt* node)
{
    ValidateConditionExprType(node->Condition());
}

void TypeChecker::PostorderVisit(ForStmt* node)
{
    // Check if the type of condition expression (if any) is bool.
    if (auto condition_expr = node->Condition())
    {
        ValidateConditionExprType(condition_expr);
    }
}

void TypeChecker::PostorderVisit(WhileStmt* node)
{
    ValidateConditionExprType(node->Condition());
}

void TypeChecker::PostorderVisit(JumpStmt* node)
{
    // TODO: implement
}

// Returns true if assigning source type value to dest type variable is possible.
bool IsBasetypeCompatible(const IBaseType* dest, const IBaseType* source)
{
    // Identical types are obviously valid.
    if (dest->ToString() == source->ToString()) return true;

    // TODO: Check if type coercion is possible.
    if (dest->ToString() == "f32" && source->ToString() == "i32") return true;

    // Otherwise, source and dest are incompatible types.
    return false;
}

bool IsNumDimensionSame(const Type& arr1, const Type& arr2)
{
    // ArraySize() returns array size for each dimension,
    // so ArraySize().size() is the number of dimensions.
    return arr1.ArraySize().size() == arr2.ArraySize().size();
}

// Returns true if value_arr has smaller or equal array size than container_arr.
// The two array types should have equal number of dimensions.
// ex) {10, 10} <- {2, 2} => true
//     {2, 2} <- {5, 1} => false (2 > 5 on the first dimension!)
bool IsArraySizeContainable(const std::vector<int>& container_arr_size, const std::vector<int>& value_arr_size)
{
    for (int i=0;i<container_arr_size.size();++i)
    {
        if (container_arr_size[i] < value_arr_size[i])
        {
            return false;
        }
    }

    return true;
}

void TypeChecker::ValidateVarDeclType(const Type& var_type, const Type& init_type, const SourcePos& decl_source_location)
{
    // Check if base type is compatible.
    if (!IsBasetypeCompatible(var_type.BaseType(), init_type.BaseType()))
    {
        auto message = std::format("implicit conversion from base type \"{}\" to \"{}\" is not allowed",
            init_type.ToString(),
            var_type.ToString()
        );
        throw SemanticError(decl_source_location, message);
    }

    // Check if initializer list has same number of dimensions.
    // ex) arr: i32[100] = {{1}, {2}} (invalid: dimension mismatch)
    if (!IsNumDimensionSame(var_type, init_type))
    {
        auto message = std::format("trying to assign expression of type \"{}\" to \"{}\" type variable",
            init_type.ToString(),
            var_type.ToString()
        );
        throw SemanticError(decl_source_location, message);
    }

    // Check if initializer list has less-or-equal size compared to the variable type.
    // ex) arr: i32[100] = {0}; (valid: partial initialization)
    if (!IsArraySizeContainable(var_type.ArraySize(), init_type.ArraySize()))
    {
        auto message = std::format("array size of initializer's type \"{}\" exceeds variable's type \"{}\"",
            init_type.ToString(),
            var_type.ToString()
        );
        throw SemanticError(decl_source_location, message);
    }
}

void TypeChecker::PostorderVisit(VarDeclStmt* node)
{
    // If we have the optional initializer, make sure the type is compatible.
    if (node->Initializer())
    {
        auto var_type = node->DeclType();
        auto init_type = GetNodeType(node->Initializer());
        auto source_location = node->Name().start_pos;

        ValidateVarDeclType(var_type, init_type, source_location);
    }

    // If we reached here without any exception,
    // the variable declaration and initializer is semantically valid.
    // Add it to the module's local symbol table.
    m_environment.AddSymbol(m_context_module_name, node, false);
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
            throw SemanticError(elem->StartPos(), message);
        }

        // Update list type to a bigger array which can store all elements in this list.
        // When number of dimensions differ between elements, an exception will be thrown.
        try
        {
            list_type.MergeArrayDim(elem_type);
        }
        catch(const std::exception&)
        {
            throw SemanticError(elem->StartPos(), "every element in an initializer list should have same dimension");
        }
    }

    list_type.AddLeftmostArrayDim(static_cast<int>(node->InitializerList().size()));

    SetNodeType(node, list_type);
}

void TypeChecker::PostorderVisit(ArrayAccessExpr* node)
{
    // TODO: implement
}

// Allowed operations:
// {+, -, *, /} between primitive types
// {+=, -=, *=, -=} between implicitly convertible types
// {==, !=} between strictly identical types
// {>, <, >=, <=} between primitive types
// {&&, ||} between bool types
void TypeChecker::PostorderVisit(BinaryExpr* node)
{
    const auto op_type = node->Operator().type;
    const auto lhs_type = GetNodeType(node->LeftHandOperand());
    const auto rhs_type = GetNodeType(node->RightHandOperand());

    // Bool type appears frequently on binary expression type check, so prepare one.
    const auto bool_type = CreatePrimiveType(TokenType::BoolType);

    // Logical and/or is allowed only between bool types
    if (op_type == TokenType::And ||
        op_type == TokenType::Or)
    {
        if (lhs_type != bool_type || rhs_type != bool_type)
        {
            const auto message = std::format("logical operator \"{}\" is only allowed between bool types, but \"{}\" and \"{}\" were given",
                node->Operator().lexeme,
                lhs_type.ToString(),
                rhs_type.ToString()
            );
            throw SemanticError(node->StartPos(), message);
        }
        
        SetNodeType(node, bool_type);
    }

    // Equality check is only allowed between strictly identical types.
    if (op_type == TokenType::Equal ||
        op_type == TokenType::NotEqual)
    {
        if (lhs_type != rhs_type)
        {
            const auto message = std::format("equality operator \"{}\" is only allowed between same types, but \"{}\" and \"{}\" were given",
                node->Operator().lexeme,
                lhs_type.ToString(),
                rhs_type.ToString()
            );
            throw SemanticError(node->StartPos(), message);
        }

        SetNodeType(node, bool_type);
    }

    // TODO: add other comparison types
    if (op_type == TokenType::Greater ||
        op_type == TokenType::Less)
    {
        // TODO: check if two types are comparable.

        SetNodeType(node, bool_type);
    }

    // TODO: add other arithmetic operator types
    if (op_type == TokenType::Divide)
    {
        // TODO: check if two types are compatible.

        // TODO: make sure that both operands are non-array primitive type.

        // TODO: if two types are different, choose more relaxed type as result.
        SetNodeType(node, lhs_type);
    }
}

void TypeChecker::PostorderVisit(FuncCallExpr* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(Identifier* node)
{
    auto symbol_name = node->ToString();

    // We are doing symbol reference!
    // Make sure it exists, and find the declaration to check the type.
    try
    {
        // This will throw exception if we try to access undefined/invisible symbol.
        auto symbol = m_environment.FindSymbol(m_context_module_name, symbol_name);
        auto type = symbol.declaration->DeclType();

        SetNodeType(node, type);
    }
    catch(const std::exception&)
    {
        auto message = std::format("trying to use undefined symbol \"{}\" in an expression",
            symbol_name
        );
        throw SemanticError(node->StartPos(), message);
    }
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
