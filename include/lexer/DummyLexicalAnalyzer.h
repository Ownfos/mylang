#ifndef MYLANG_DUMMY_LEXICAL_ANALYZER_H
#define MYLANG_DUMMY_LEXICAL_ANALYZER_H

#include "common/IStream.h"
#include "lexer/Token.h"
#include <vector>

namespace mylang
{

class DummyLexicalAnalyzer : public IStream<Token>
{
public:
    DummyLexicalAnalyzer(const std::vector<Token>& tokens);

    virtual bool IsFinished() const override;
    virtual Token GetNext() override;

private:
    std::vector<Token> m_tokens;

    // GetNext() increments index before accessing m_tokens.
    // In order to make the first GetNext() call return m_tokens[0],
    // the initial value of index should be set to -1.
    int m_current_token_ind = -1;
};

} // namespace mylang

#endif // MYLANG_DUMMY_LEXICAL_ANALYZER_H
