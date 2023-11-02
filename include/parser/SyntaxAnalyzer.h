#ifndef MYLANG_SYNTAX_ANALYZER_H
#define MYLANG_SYNTAX_ANALYZER_H

#include "lexer/ILexicalAnalyzer.h"
#include "parser/ast/IAbstractSyntaxTree.h"
#include <memory>

namespace mylang
{

// Generate an abstract syntax tree from given stream of tokens.
class SyntaxAnalyzer
{
public:
    SyntaxAnalyzer(std::unique_ptr<ILexicalAnalyzer>&& lexer);

    std::shared_ptr<IAbstractSyntaxTree> GenerateAST();

private:
    std::unique_ptr<ILexicalAnalyzer> m_lexer;
};

} // namespace mylang

#endif // MYLANG_SYNTAX_ANALYZER_H
