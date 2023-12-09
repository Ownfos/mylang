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

void TypeChecker::ValidateTypeExistence(const Type& type, std::string_view who, const SourcePos& where)
{
    if (!type.IsValid(m_environment, m_context_module_name))
    {
        auto message = std::format("{} tried to use invalid type \"{}\"",
            who,
            type.ToString()
        );
        throw SemanticError(where, message);
    }
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
    auto who = std::format("return type of function \"{}\"", node->Name().lexeme);
    ValidateTypeExistence(node->ReturnType(), who, node->StartPos());

    m_environment.OpenScope(m_context_module_name);
}

void TypeChecker::PostorderVisit(FuncDecl* node)
{
    m_environment.CloseScope(m_context_module_name);
}

void TypeChecker::PreorderVisit(Parameter* node)
{
    // Throw an error if the type is invalid in this module's context.
    auto who = std::format("parameter \"{}\"", node->Name().lexeme);
    ValidateTypeExistence(node->DeclType(), who, node->StartPos());

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
        auto who = std::format("member variable \"{}\" of struct \"{}\"",
            member.name.lexeme,
            node->Name().lexeme
        );
        ValidateTypeExistence(member.type, who, member.name.start_pos);
    }
}

void TypeChecker::PreorderVisit(CompoundStmt* node)
{
    m_environment.OpenScope(m_context_module_name);
}

void TypeChecker::PostorderVisit(CompoundStmt* node)
{
    m_environment.CloseScope(m_context_module_name);
}

// Throws an exception if 'type' is different from 'expected'.
// 'who' and 'where' are used to provide information to the SemanticError.
void ValidateTypeEquality(const Type& type, const Type& expected, std::string_view who, const SourcePos& where)
{
    if (type != expected)
    {
        auto message = std::format("expected type of {} is \"{}\", but \"{}\" was given",
            who,
            expected.ToString(),
            type.ToString()
        );
        throw SemanticError(where, message);
    }
}

void TypeChecker::ValidateConditionExprType(const Expr* condition_expr)
{
    const auto& type = GetExprTrait(condition_expr).type;
    const auto& expected = CreatePrimiveType(TokenType::BoolType);
    ValidateTypeEquality(type, expected, "a condition expression", condition_expr->StartPos());
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
    if (node->JumpType().type == TokenType::Return)
    {
        // In case of empty return statement (i.e. "return;"),
        // use void type as the expression's type.
        auto ret_expr = node->ReturnValueExpr();
        auto ret_type = (ret_expr == nullptr) ? CreateVoidType() : GetExprTrait(ret_expr).type;

        // Check if the return type matches the function signature.
        auto who = std::format("return statement inside function \"{}\"",
            m_current_function->Name().lexeme
        );
        ValidateTypeEquality(ret_type, m_current_function->ReturnType(), who, node->StartPos());
    }
}

// Returns true if assigning source type value to dest type variable is possible.
bool IsBasetypeCompatible(const IBaseType* dest, const IBaseType* source)
{
    // Identical types are obviously valid.
    if (dest->ToString() == source->ToString()) return true;

    // Check if type coercion is possible.
    if (dest->ToString() == "f32" && source->ToString() == "i32") return true;

    // Otherwise, source and dest are incompatible types.
    return false;
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

void ValidateBasetypeCompatibility(const IBaseType* dest, const IBaseType* source, const SourcePos& where)
{
    if (!IsBasetypeCompatible(dest, source))
    {
        auto message = std::format("implicit conversion from base type \"{}\" to \"{}\" is not allowed",
            source->ToString(),
            dest->ToString()
        );
        throw SemanticError(where, message);
    }
}

void ValidateNumDimensionEquality(const Type& lhs, const Type& rhs, const SourcePos& where)
{
    if (lhs.NumDimensions() != rhs.NumDimensions())
    {
        auto message = std::format("number of dimensions differ between types \"{}\" and \"{}\"",
            lhs.ToString(),
            rhs.ToString()
        );
        throw SemanticError(where, message);
    }
}

void ValidateInitializerListSize(const Type& var_type, const Type& init_type, const SourcePos& where)
{
    if (!IsArraySizeContainable(var_type.ArraySize(), init_type.ArraySize()))
    {
        auto message = std::format("array size of initializer's type \"{}\" exceeds variable's type \"{}\"",
            init_type.ToString(),
            var_type.ToString()
        );
        throw SemanticError(where, message);
    }
}

// Check if we can initialize variable of var_type with value of init_type.
// If not, semantic error will be thrown.
// 'decl_source_location' is used on the exception message,
// to show where the error happened.
void ValidateVarDeclType(const Type& var_type, const Type& init_type, const SourcePos& where)
{
    // Check if we can assign initializer to variable,
    // while only considering the base type.
    ValidateBasetypeCompatibility(var_type.BaseType(), init_type.BaseType(), where);

    // Check if initializer list has same number of dimensions.
    // ex) arr: i32[100] = {{1}, {2}} (invalid: dimension mismatch)
    ValidateNumDimensionEquality(var_type, init_type, where);

    // Check if initializer list has less-or-equal size compared to the variable type.
    // ex) arr: i32[100] = {0}; (valid: partial initialization)
    ValidateInitializerListSize(var_type, init_type, where);
}

void TypeChecker::PostorderVisit(VarDeclStmt* node)
{
    // Make sure a valid type is used.
    auto who = std::format("local variable \"{}\"", node->Name().lexeme);
    auto where = node->StartPos();
    auto var_type = node->DeclType();
    ValidateTypeExistence(var_type, who, where);

    // If we have the optional initializer, make sure the type is compatible.
    if (node->Initializer())
    {
        auto init_type = GetExprTrait(node->Initializer()).type;
        ValidateVarDeclType(var_type, init_type, where);
    }

    // If we reached here without any exception,
    // the variable declaration and initializer is semantically valid.
    // Add it to the module's local symbol table.
    m_environment.AddSymbol(m_context_module_name, node, false);
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
    const auto index_expr = node->Index();

    // Index should have int type.
    const auto& index_type = GetExprTrait(index_expr).type;
    auto expected = CreatePrimiveType(TokenType::IntType);
    auto where = index_expr->StartPos();
    ValidateTypeEquality(index_type, expected, "an array index", where);

    // Operand should be an array type.
    const auto& [is_operand_lvalue, operand_type] = GetExprTrait(node->Operand());
    if (!operand_type.IsArray())
    {
        auto message = std::format("array access operator [] cannot be applied to non-array type \"{}\"",
            operand_type.ToString()
        );
        throw SemanticError(where, message);
    }

    // Record the result type.
    auto result_type = operand_type; // copy Type instance
    result_type.RemoveLeftmostArrayDim();

    SetExprTrait(node, result_type, is_operand_lvalue);
}

// Throws exception if the type was an array.
void ValidateTypeIsNotArray(const Type& type, std::string_view who, const SourcePos& where)
{
    if (type.IsArray())
    {
        auto message = std::format("expected a non-array type for {}, but \"{}\" was given",
            who,
            type.ToString()
        );
        throw SemanticError(where, message);
    }
}

bool IsTypeNumeric(const Type& type)
{
    if (type == CreatePrimiveType(TokenType::IntType)) return true;
    if (type == CreatePrimiveType(TokenType::FloatType)) return true;

    return false;
}

// Throws exception if given type is not a numeric type (i32 or f32)·
void ValidateTypeIsNumeric(const Type& type, std::string_view who, const SourcePos& where)
{
    if (!IsTypeNumeric(type))
    {
        const auto message = std::format("expected numeric type for {}, but \"{}\" was given",
            who,
            type.ToString()
        );
        throw SemanticError(where, message);
    }
}

void ThrowInvalidOperationError(const Type& lhs_type, const Type& rhs_type, const Token& op_token)
{
    auto message = std::format("operation \"{}\" {} \"{}\" is not allowed",
        lhs_type.ToString(),
        op_token.lexeme,
        rhs_type.ToString()
    );
    throw SemanticError(op_token.start_pos, message);
}

// If the operation is valid, return the result type.
// If not, throw a semantic exception.
Type FindArithmeticResultType(const Type& lhs_type, const Type& rhs_type, const Token& op_token)
{
    // Arithmetic operators are not applicable to array types.
    auto who = std::format("arithmetic operator {}", op_token.lexeme);
    auto where = op_token.start_pos;
    ValidateTypeIsNotArray(lhs_type, who, where);
    ValidateTypeIsNotArray(rhs_type, who, where);
    
    // Now find if there exists a type pair for this operation.
    const auto int_type = CreatePrimiveType(TokenType::IntType);
    const auto float_type = CreatePrimiveType(TokenType::FloatType);
    const auto str_type = CreatePrimiveType(TokenType::StringType);

    // The only allowed operation between two strings is concatenation.
    auto is_concatenation = op_token.type == TokenType::Plus || op_token.type == TokenType::PlusAssign;
    if (lhs_type == str_type && rhs_type == str_type && is_concatenation)
    {
        return str_type;
    }
    // Operation between float or int.
    else if (IsTypeNumeric(lhs_type))
    {
        auto who = std::format("right hand operand of arithmetic operator {}", op_token.lexeme);
        ValidateTypeIsNumeric(rhs_type, who, where);

        // Take type coercion into account.
        if (lhs_type == int_type && rhs_type == int_type)
        {
            return int_type;
        }
        else
        {
            return float_type;
        }
    }
    // All other operations are invalid.
    else
    {
        ThrowInvalidOperationError(lhs_type, rhs_type, op_token);
    }
}

// Throws exception if inequality operator (>, >=, <, <=)
// cannot be applied between two types.
void ValidateBaseTypeInequalityComparable(const Type& lhs_type, const Type& rhs_type, const Token& op_token)
{
    // Comparision between float and int is possible.
    if (IsTypeNumeric(lhs_type) && IsTypeNumeric(rhs_type)) return;

    // Comparison between two strings is also allowed (dictionary order!)
    auto str_type = CreatePrimiveType(TokenType::StringType);
    if (lhs_type == str_type && rhs_type == str_type) return;

    // Otherwise, we cannot compare two types using inequality operators.
    ThrowInvalidOperationError(lhs_type, rhs_type, op_token);
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

    // These instances frequently appear within this function,
    // so why not prepare them in advance!
    // Note: 'where' is used to report source location when semantic error occurs
    const auto& bool_type = CreatePrimiveType(TokenType::BoolType);
    const auto& where = op_token.start_pos;

    // Logical and/or is allowed only between bool types
    if (op_type == TokenType::And ||
        op_type == TokenType::Or)
    {
        auto who = std::format("operand of logical operator {}", op_token.lexeme);
        ValidateTypeEquality(lhs_type, bool_type, who, where);
        ValidateTypeEquality(rhs_type, bool_type, who, where);
        
        SetExprTrait(node, bool_type);
    }

    // Equality check is only allowed between strictly identical types.
    if (op_type == TokenType::Equal ||
        op_type == TokenType::NotEqual)
    {
        auto who = std::format("right hand operand of equality operator {}", op_token.lexeme);
        ValidateTypeEquality(rhs_type, lhs_type, who, where);

        SetExprTrait(node, bool_type);
    }

    // Inequality check is only allowed between non-array primitive types
    if (op_type == TokenType::Greater ||
        op_type == TokenType::GreaterEqual ||
        op_type == TokenType::Less ||
        op_type == TokenType::LessEqual)
    {
        auto who = std::format("operand of inequality operator {}", op_token.lexeme);
        ValidateTypeIsNotArray(lhs_type, who, where);
        ValidateTypeIsNotArray(rhs_type, who, where);
        ValidateBaseTypeInequalityComparable(lhs_type, rhs_type, op_token);

        SetExprTrait(node, bool_type);
    }

    if (op_type == TokenType::Divide ||
        op_type == TokenType::Multiply ||
        op_type == TokenType::Plus ||
        op_type == TokenType::Minus)
    {

        // Not all operands have corresponding arithmetic operator.
        // For example, we cannot add an a number to a string.
        // This will throw an exception in case of invalid operation.
        auto result_type = FindArithmeticResultType(lhs_type, rhs_type, op_token);

        SetExprTrait(node, result_type);
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

        // Simple assignment "="
        if (op_type == TokenType::Assign)
        {
            // Array assignment requires strict type identity.
            if (lhs_type.IsArray())
            {
                auto who = "right hand operand of array assignment operator";
                ValidateTypeEquality(rhs_type, lhs_type, who, where);
            }
            // For non-array types, we need to check if implicit conversion is possible.
            else
            {
                ValidateBasetypeCompatibility(lhs_type.BaseType(), rhs_type.BaseType(), op_token.start_pos);
            }
            SetExprTrait(node, lhs_type, true);
        }
        // Arithmetic assignments like "+="
        else
        {
            auto result_type = FindArithmeticResultType(lhs_type, rhs_type, op_token);
            SetExprTrait(node, lhs_type);
        }
    }
}

// Throws exception if number of arguments differ from requires parameters.
void ValidateArgumentNumber(
    const std::vector<ParamType>& param_types,
    const std::vector<std::shared_ptr<Expr>>& args,
    const SourcePos& where
)
{
    if (param_types.size() != args.size())
    {
        const auto message = std::format("the function only requires {} arguments, but {} was given",
            param_types.size(),
            args.size()
        );
        throw SemanticError(where, message);
    }
}

bool IsLValueRequired(const ParamUsage& usage)
{
    return (usage == ParamUsage::InOut) || (usage == ParamUsage::Out);
}

// Throws exception if the expression is an rvalue.
void TypeChecker::ValidateLValueQualifier(const Expr* expr, std::string_view who, const SourcePos& where)
{
    if (!GetExprTrait(expr).is_lvalue)
    {
        const auto message = std::format("expected an lvalue for {}, but an rvalue was given", who);
        throw SemanticError(where, message);
    }
}

// Try to cast the type to a non-array FuncType.
// Throws exception if the type was not a callable type.
const FuncType* TryTypecastToFuncType(const Type& type, const SourcePos& where)
{
    // An array type, regardless of its base type, is not callable.
    ValidateTypeIsNotArray(type, "a callee expression", where);

    // Make sure the base type is FuncType.
    auto base_type = dynamic_cast<const FuncType*>(type.BaseType());
    if (base_type == nullptr)
    {
        const auto message = std::format("\"{}\" is not a callable type",
            type.ToString()
        );
        throw SemanticError(where, message);
    }
    return base_type;
}

void TypeChecker::PostorderVisit(FuncCallExpr* node)
{
    // Check if the callee has a callable type.
    auto where = node->StartPos();
    const Type& callee_node_type = GetExprTrait(node->Function()).type;
    const FuncType* func_type = TryTypecastToFuncType(callee_node_type, where);

    // Check if the number of arguments is valid.
    const auto& param_types = func_type->ParamTypes();
    const auto& args = node->ArgumentList();
    ValidateArgumentNumber(param_types, args, where);

    // Check if each argument has a valid type.
    for (int i = 0; i < args.size(); ++i)
    {
        // Alias for frequently used variables.
        const auto& param = param_types[i];
        auto arg_expr = args[i].get();

        // Make sure the types match.
        auto who = std::format("argument {}", i);
        auto where = arg_expr->StartPos();
        const auto& type = GetExprTrait(arg_expr).type;
        ValidateTypeEquality(type, param.type, who, where);

        // Make sure the argument is an lvalue
        // if the parameter usage is 'out' or 'inout'.
        if (IsLValueRequired(param.usage))
        {
            auto who = std::format("parameter type \"{}\"", param.ToString());
            ValidateLValueQualifier(arg_expr, who, where);
        }
    }

    // Since everything is good to go, register the type as the function's return type.
    SetExprTrait(node, func_type->ReturnType());
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

const StructDecl* TypeChecker::TryToFindStructTypeDecl(const Type& type, const SourcePos& where)
{
    try
    {
        // There are three possible cases:
        // 1. 'type' is a non-struct type, such as primitives and functions.
        // 2. 'type' is a struct type.
        // Note that invalid struct types cannot reach here,
        // because all variable types are validated while visiting VarDeclStmt.
        const auto& struct_decl_symbol = m_environment.FindSymbol(m_context_module_name, type.ToString());
        return dynamic_cast<const StructDecl*>(struct_decl_symbol.declaration);
    }
    catch(const std::exception&)
    {
        auto message = std::format("\"{}\" is not a struct type",
            type.ToString()
        );
        throw SemanticError(where, message);
    }
}

void TypeChecker::PostorderVisit(MemberAccessExpr* node)
{
    // Check if the operand is really a struct type.
    const auto& [is_struct_lvalue, struct_type] = GetExprTrait(node->Struct());
    const StructDecl* struct_decl = TryToFindStructTypeDecl(struct_type, node->StartPos());

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
    auto message = std::format("struct type \"{}\" does not have member variable named \"{}\"",
        struct_type.ToString(),
        member_name.lexeme
    );
    throw SemanticError(member_name.start_pos, message);
}

void TypeChecker::PostorderVisit(PrefixExpr* node)
{
    auto op = node->Operator();
    auto operand_expr = node->Operand();
    auto operand_type = GetExprTrait(operand_expr).type;

    // Commonly used information on error reports.
    auto who = std::format("operand of unary operator {}", op.lexeme);
    auto where = node->StartPos();

    // Unary +/- is only allowed on numeric types.
    if (op.type == TokenType::Plus ||
        op.type == TokenType::Minus)
    {
        ValidateTypeIsNumeric(operand_type, who, where);
    }
    // Unary ! is only allowed on bool type.
    else if (op.type == TokenType::Not)
    {
        auto bool_type = CreatePrimiveType(TokenType::BoolType);
        ValidateTypeEquality(operand_type, bool_type, who, where);
    }
    // Prefix ++/-- is only allowed on lvalue int type
    else
    {
        auto int_type = CreatePrimiveType(TokenType::IntType);
        ValidateTypeEquality(operand_type, int_type, who, where);
        ValidateLValueQualifier(operand_expr, who, where);
    }

    // Prefix operators do not change the type.
    SetExprTrait(node, operand_type);
}

void TypeChecker::PostorderVisit(PostfixExpr* node)
{
    auto op = node->Operator();
    auto operand_expr = node->Operand();
    auto operand_type = GetExprTrait(operand_expr).type;

    auto int_type = CreatePrimiveType(TokenType::IntType);
    auto who = std::format("operand of unary operator {}", op.lexeme);
    auto where = op.start_pos;
    
    // Postfix ++/-- is only allowed on lvalue int type
    ValidateTypeEquality(operand_type, int_type, who, where);
    ValidateLValueQualifier(operand_expr, who, where);

    // Postfix operators do not change the type.
    SetExprTrait(node, operand_type);
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
