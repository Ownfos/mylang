#include "parser/type/base/StructType.h"
#include "parser/ProgramEnvironment.h"
#include "parser/ast/globdecl/StructDecl.h"

namespace mylang
{

StructType::StructType(const Token& type)
    : m_type(type)
{}

std::string StructType::ToString() const
{
    return m_type.lexeme;
}

bool StructType::IsValid(
    ProgramEnvironment& environment,
    std::string_view context_module_name
) const
{
    try
    {
        // Check if the base type's name exists in a symbol table.
        // If not, an exception will be thrown.
        auto symbol = environment.FindSymbol(context_module_name, m_type.lexeme); 

        // If the symbol exists, make sure it was declared as a struct type.
        //
        // Since syntax analyzer doesn't care
        // whether an identifier is a struct or not,
        // the test below can actually fail!
        //
        // example)
        // foo: func = (){}
        // main: func = ()->int {
        //     i: foo = 1; // ERROR: foo is not a struct type!
        // }
        return dynamic_cast<StructDecl*>(symbol.declaration) != nullptr;
    }
    catch(const std::exception&)
    {
        return false;
    }
}

} // namespace mylang
