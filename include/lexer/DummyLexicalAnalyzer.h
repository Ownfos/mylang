#ifndef MYLANG_DUMMY_LEXICAL_ANALYZER_H
#define MYLANG_DUMMY_LEXICAL_ANALYZER_H

#include "lexer/ILexicalAnalyzer.h"
#include <vector>

namespace mylang
{

class DummyLexicalAnalyzer : public ILexicalAnalyzer
{
public:
    DummyLexicalAnalyzer(const std::vector<Token>& tokens);

    virtual Token GetNextToken() override;

private:
    std::vector<Token> m_tokens;
    int m_next_token_ind = 0;
};

} // namespace mylang

#endif // MYLANG_DUMMY_LEXICAL_ANALYZER_H
