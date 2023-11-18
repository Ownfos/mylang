#include "lexer/Token.h"
#include <exception>

namespace mylang
{

class SemanticError : public std::exception
{
public:
    SemanticError(const SourcePos& location, std::string_view message);
    
    const SourcePos& where() const;
    virtual const char* what() const override;

private:
    SourcePos m_location;
    std::string m_message;
};

} // namespace mylang
