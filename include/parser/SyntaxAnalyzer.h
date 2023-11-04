#ifndef MYLANG_SYNTAX_ANALYZER_H
#define MYLANG_SYNTAX_ANALYZER_H

#include "common/IStream.h"
#include "lexer/Token.h"
#include "parser/ast/IAbstractSyntaxTree.h"
#include <memory>

namespace mylang
{

// Generate an abstract syntax tree from given stream of tokens.
class SyntaxAnalyzer
{
public:
    SyntaxAnalyzer(std::unique_ptr<IStream<Token>>&& lexer);

    std::shared_ptr<IAbstractSyntaxTree> GenerateAST();

private:
    std::unique_ptr<IStream<Token>> m_lexer;
};

} // namespace mylang

#endif // MYLANG_SYNTAX_ANALYZER_H
