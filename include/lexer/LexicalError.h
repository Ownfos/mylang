#ifndef MYLANG_LEXICAL_ERROR_H
#define MYLANG_LEXICAL_ERROR_H

#include "lexer/Token.h"
#include <exception>

namespace mylang
{

class LexicalError : public std::exception
{
public:
    LexicalError(const SourcePos& location, const std::string& message);
    
    const SourcePos& where() const;
    virtual const char* what() const override;

private:
    SourcePos m_location;
    std::string m_message;
};

} // namespace mylang


#endif // MYLANG_LEXICAL_ERROR_H
