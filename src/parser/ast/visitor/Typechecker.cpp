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
    auto type = GetExprTrait(condition_expr).type;
    if (type != CreatePrimiveType(TokenType::BoolType))
    {
        auto message = std::format("condition expression should have bool type, instead of \"{}\"",
            type.ToString()
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

void ValidateBasetypeCompatibility(const IBaseType* dest, const IBaseType* source, const SourcePos& error_report_location)
{
    if (!IsBasetypeCompatible(dest, source))
    {
        auto message = std::format("implicit conversion from base type \"{}\" to \"{}\" is not allowed",
            source->ToString(),
            dest->ToString()
        );
        throw SemanticError(error_report_location, message);
    }
}

void ValidateNumDimensionEquality(const Type& lhs, const Type& rhs, const SourcePos& error_report_location)
{
    if (!IsNumDimensionSame(lhs, rhs))
    {
        auto message = std::format("number of dimensions differ between types \"{}\" and \"{}\"",
            lhs.ToString(),
            rhs.ToString()
        );
        throw SemanticError(error_report_location, message);
    }
}

void ValidateInitializerListSize(const Type& var_type, const Type& init_type, const SourcePos& error_report_location)
{
    if (!IsArraySizeContainable(var_type.ArraySize(), init_type.ArraySize()))
    {
        auto message = std::format("array size of initializer's type \"{}\" exceeds variable's type \"{}\"",
            init_type.ToString(),
            var_type.ToString()
        );
        throw SemanticError(error_report_location, message);
    }
}

void TypeChecker::ValidateVarDeclType(const Type& var_type, const Type& init_type, const SourcePos& decl_source_location)
{
    // Check if we can assign initializer to variable,
    // while only considering the base type.
    ValidateBasetypeCompatibility(var_type.BaseType(), init_type.BaseType(), decl_source_location);

    // Check if initializer list has same number of dimensions.
    // ex) arr: i32[100] = {{1}, {2}} (invalid: dimension mismatch)
    ValidateNumDimensionEquality(var_type, init_type, decl_source_location);

    // Check if initializer list has less-or-equal size compared to the variable type.
    // ex) arr: i32[100] = {0}; (valid: partial initialization)
    ValidateInitializerListSize(var_type, init_type, decl_source_location);
}

void TypeChecker::PostorderVisit(VarDeclStmt* node)
{
    // If we have the optional initializer, make sure the type is compatible.
    if (node->Initializer())
    {
        auto var_type = node->DeclType();
        auto init_type = GetExprTrait(node->Initializer()).type;
        ValidateVarDeclType(var_type, init_type, node->StartPos());
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
    SetExprTrait(node, GetExprTrait(node->Expression()).type);
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
    auto first_elem = node->InitializerList().begin()->get();
    auto list_type = GetExprTrait(first_elem).type;

    for (const std::shared_ptr<VarInit>& elem : node->InitializerList())
    {
        auto elem_type = GetExprTrait(elem.get()).type;

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

    SetExprTrait(node, list_type);
}

void TypeChecker::PostorderVisit(ArrayAccessExpr* node)
{
    // Index should have int type.
    const auto index_expr = node->Index();
    const auto& index_type = GetExprTrait(index_expr).type;
    if (index_type != CreatePrimiveType(TokenType::IntType))
    {
        auto message = std::format("array index should have type i32, but \"{}\" was given",
            index_type.ToString()
        );
        throw SemanticError(index_expr->StartPos(), message);
    }

    // Operand should be an array type.
    const auto& operand_type = GetExprTrait(node->Operand()).type;
    if (!operand_type.IsArray())
    {
        auto message = std::format("array access operator [] cannot be applied to non-array type \"{}\"",
            operand_type.ToString()
        );
        throw SemanticError(index_expr->StartPos(), message);
    }

    // Record the result type.
    auto result_type = operand_type; // copy Type instance
    result_type.RemoveLeftmostArrayDim();

    SetExprTrait(node, result_type);
}

// Allowed operations:
// {+, -, *, /} between primitive types
// {+=, -=, *=, -=} between implicitly convertible types
// {==, !=} between strictly identical types
// {>, <, >=, <=} between primitive types
// {&&, ||} between bool types
void TypeChecker::PostorderVisit(BinaryExpr* node)
{
    const auto& op_token = node->Operator();
    const auto& op_type = op_token.type;
    const auto& [is_lhs_lvalue, lhs_type] = GetExprTrait(node->LeftHandOperand());
    const auto& rhs_type = GetExprTrait(node->RightHandOperand()).type;

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
            throw SemanticError(op_token.start_pos, message);
        }
        
        SetExprTrait(node, bool_type);
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
            throw SemanticError(op_token.start_pos, message);
        }

        SetExprTrait(node, bool_type);
    }

    // TODO: add other comparison types
    if (op_type == TokenType::Greater ||
        op_type == TokenType::Less)
    {
        // TODO: check if two types are comparable.

        SetExprTrait(node, bool_type);
    }

    if (op_type == TokenType::Divide ||
        op_type == TokenType::Multiply ||
        op_type == TokenType::Plus ||
        op_type == TokenType::Minus)
    {
        // TODO: check if two types are compatible.

        // TODO: make sure that both operands are non-array primitive type.

        // TODO: if two types are different, choose more relaxed type as result.
        SetExprTrait(node, lhs_type);
    }

    if (op_type == TokenType::Assign ||
        op_type == TokenType::PlusAssign ||
        op_type == TokenType::MinusAssign ||
        op_type == TokenType::MultiplyAssign ||
        op_type == TokenType::DivideAssign)
    {
        // Assignment requires lhs to be an lvalue.
        if (!is_lhs_lvalue)
        {
            throw SemanticError(op_token.start_pos, "assignment to an rvalue is not allowed");
        }

        // Array assignment requires strict type identity.
        if (lhs_type.IsArray() && (lhs_type != rhs_type))
        {
            const auto message = std::format("assignment operator \"{}\" is only allowed between same types, but \"{}\" and \"{}\" were given",
                node->Operator().lexeme,
                lhs_type.ToString(),
                rhs_type.ToString()
            );
            throw SemanticError(op_token.start_pos, message);
        }

        // For non-array types, we need to check if implicit conversion is possible.
        ValidateBasetypeCompatibility(lhs_type.BaseType(), rhs_type.BaseType(), op_token.start_pos);

        // For assignment other than plain "=",
        // check if the corresponding arithmetic operation
        // between lhs and rhs is possible.
        if (op_type != TokenType::Assign)
        {
            // TODO: throw exception if not possible
        }

        SetExprTrait(node, lhs_type);
    }
}

void TypeChecker::PostorderVisit(FuncCallExpr* node)
{
    const auto& func_type = GetExprTrait(node->Function()).type;

    // Throw an error if basetype is non-callable, or the operand is an array.
    // Note: The only thing we allow is a callble, non-array instance!
    auto base_type = dynamic_cast<const FuncType*>(func_type.BaseType());
    if (base_type == nullptr || func_type.IsArray())
    {
        const auto message = std::format("\"{}\" is not a callable type",
            func_type.ToString()
        );
        throw SemanticError(node->StartPos(), message);
    }

    // Check if the number of arguments is valid.
    // TODO: if possible, find other way that doesn't use dynamic_cast...
    const auto& param_types = base_type->ParamTypes();
    const auto& args = node->ArgumentList();
    if (param_types.size() != args.size())
    {
        const auto message = std::format("the function only requires {} arguments, but {} was given",
            param_types.size(),
            args.size()
        );
        throw SemanticError(node->StartPos(), message);
    }

    // Check if each argument has a valid type.
    for (int i = 0; i < args.size(); ++i)
    {
        // This is the one passed as an argument.
        const auto& [is_arg_lvalue, arg_type] = GetExprTrait(args[i].get());

        // This is the parameter signature.
        const auto& [expected_type, param_usage] = param_types[i];

        // If two doesn't match, throw a semantic error.
        if (arg_type != expected_type)
        {
            const auto message = std::format("expected argument type \"{}\", but \"{}\" was given",
                expected_type.ToString(),
                arg_type.ToString()
            );
            throw SemanticError(args[i]->StartPos(), message);
        }

        // 'inout' or 'out' parameters require corresponding argument to be an lvalue.
        auto is_lvalue_required = (param_usage == ParamUsage::InOut) || (param_usage == ParamUsage::Out);
        if (is_lvalue_required && !is_arg_lvalue)
        {
            const auto message = std::format("an rvalue cannot be passed as parameter type \"{}\"",
                param_types[i].ToString()
            );
            throw SemanticError(args[i]->StartPos(), message);
        }
    }

    // Since everything is good to go, register the type as the function's return type.
    // TODO: handle void type
    if (auto ret_type = base_type->ReturnType())
    {
        SetExprTrait(node, ret_type.value());
    }
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

        // Note: the third parameter denotes that this is an lvalue.
        SetExprTrait(node, type, true);
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
    SetExprTrait(node, node->DeclType());
}

void TypeChecker::PostorderVisit(MemberAccessExpr* node)
{
    auto [is_struct_lvalue, struct_type] = GetExprTrait(node->Struct());

    // An array is obviously not a struct.
    if (struct_type.IsArray())
    {
        // TODO: throw semantic error: member access cannot be applied to an array type \"{}\"
    }

    // Check if the operand is really a struct type.
    const StructDecl* struct_decl;
    try
    {
        const auto& struct_decl_symbol = m_environment.FindSymbol(m_context_module_name, struct_type.ToString());

        // FindSymbol() will throw an exception for any non-struct type,
        // so if it didn't crash, this cast is guaranteed to succeed.
        struct_decl = dynamic_cast<const StructDecl*>(struct_decl_symbol.declaration);
    }
    catch(const std::exception&)
    {
        auto message = std::format("\"{}\" is not a struct type",
            struct_type.ToString()
        );
        throw SemanticError(node->StartPos(), message);
    }

    // Check if the struct has a member with matching name.
    const auto& member_name = node->MemberName();
    for (const MemberVariable& member : struct_decl->Members())
    {
        if (member.name.lexeme == member_name.lexeme)
        {
            // Note: if the operand is an lvalue, the member variable is also an lvalue.
            SetExprTrait(node, member.type, is_struct_lvalue);
            return;
        }
    }

    // Reaching this line implies that we failed to find a matching member name.
    auto message = std::format("struct type \"person\" does not have member variable named \"height\"",
        struct_type.ToString(),
        member_name.lexeme
    );
    throw SemanticError(member_name.start_pos, message);
}

void TypeChecker::PostorderVisit(PostfixExpr* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(PrefixExpr* node)
{
    // TODO: implement
}

void TypeChecker::SetExprTrait(const IAbstractSyntaxTree* node, const Type& type, bool is_lvalue)
{
    m_expr_trait_dict.insert({node, ExprTrait{is_lvalue, type}});
}

const ExprTrait& TypeChecker::GetExprTrait(const IAbstractSyntaxTree* node) const
{
    return m_expr_trait_dict.at(node);
}

} // namespace mylang
