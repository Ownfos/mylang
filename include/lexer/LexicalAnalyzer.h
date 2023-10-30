#ifndef MYLANG_LEXICAL_ANALYZER_H
#define MYLANG_LEXICAL_ANALYZER_H

#include "lexer/ILexicalAnalyzer.h"
#include "file/ISourceFile.h"
#include <memory>

namespace mylang
{

class LexicalAnalyzer : public ILexicalAnalyzer
{
public:
    LexicalAnalyzer(std::unique_ptr<ISourceFile>&& source_file);

    virtual Token GetNextToken() override;
};

} // namespace mylang


#endif // MYLANG_LEXICAL_ANALYZER_H
