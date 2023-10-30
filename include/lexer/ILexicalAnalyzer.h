#ifndef MYLANG_I_LEXICAL_ANALYZER_H
#define MYLANG_I_LEXICAL_ANALYZER_H

#include "lexer/Token.h"

namespace mylang
{

// Interface for source code -> token converters
class ILexicalAnalyzer
{
public:
    virtual ~ILexicalAnalyzer() = default;

    virtual Token GetNextToken() = 0;
};

} // namespace mylang

#endif // MYLANG_I_LEXICAL_ANALYZER_H
