#ifndef MYLANG_SYNTAX_ANALYZER_H
#define MYLANG_SYNTAX_ANALYZER_H

#include "common/BufferedStream.h"
#include "parser/routine/ModuleParser.h"
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
    std::shared_ptr<BufferedStream<Token>> m_lexer;
    std::unique_ptr<ModuleParser> m_parser;
};

} // namespace mylang

#endif // MYLANG_SYNTAX_ANALYZER_H
