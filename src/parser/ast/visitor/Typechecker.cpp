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

void TypeChecker::ValidateType(const Type& type)
{
    auto base_type = type.BaseType();
    if (auto type = dynamic_cast<const StructType*>(base_type))
    {
        ValidateStructType(type);
    }
    else if (auto type = dynamic_cast<const FuncType*>(base_type))
    {
        ValidateFuncType(type);
    }
}

void TypeChecker::ValidateStructType(const StructType* type)
{
    // Check if the base type's name exists in a symbol table.
    // If not, an exception will be thrown.
    auto symbol = m_environment.FindSymbol(m_context_module_name, type->ToString()); 

    // If the symbol exists, make sure it was declared as a struct type.
    if (!dynamic_cast<StructDecl*>(symbol.declaration))
    {
        throw std::exception("a non-struct type identifier was used as a type specifier");
    }
}

// Recursive validation on return type and parameters.
void TypeChecker::ValidateFuncType(const FuncType* type)
{
    if (auto ret_type = type->ReturnType())
    {
        ValidateType(ret_type.value());
    }

    for (const ParamType& param_type : type->ParamTypes())
    {
        ValidateType(param_type.type);
    }
}

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
        try
        {
            ValidateType(member.type);
        }
        catch(const std::exception&)
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
    // TODO: implement
}

void TypeChecker::PostorderVisit(ExprStmt* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(VarInitExpr* node)
{
    // TODO: implement
}

void TypeChecker::PostorderVisit(VarInitList* node)
{
    // TODO: implement
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
    // TODO: implement
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

} // namespace mylang
