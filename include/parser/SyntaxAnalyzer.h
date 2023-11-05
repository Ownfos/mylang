#ifndef MYLANG_SYNTAX_ANALYZER_H
#define MYLANG_SYNTAX_ANALYZER_H

#include "common/BufferedStream.h"
#include "lexer/Token.h"
#include "parser/ast/IAbstractSyntaxTree.h"
#include <memory>
#include <optional>
#include <set>

namespace mylang
{

class Program;
class FuncDecl;
class StructDecl;
struct Parameter;

// Generate an abstract syntax tree from given stream of tokens.
class SyntaxAnalyzer
{
public:
    SyntaxAnalyzer(std::unique_ptr<IStream<Token>>&& lexer);

    std::shared_ptr<IAbstractSyntaxTree> GenerateAST();

private:
    Token Accept(TokenType type);
    Token AcceptOneOf(const std::set<TokenType>& types);
    std::optional<Token> OptionalAccept(TokenType type);
    std::optional<Token> OptionalAcceptOneOf(const std::set<TokenType>& types);

    std::shared_ptr<Program> ParseProgram();
    std::shared_ptr<FuncDecl> ParseFuncDecl(bool should_export, Token name);
    std::vector<Parameter> ParseParamList();
    Parameter ParseParam();
    Token ParseType();
    std::shared_ptr<StructDecl> ParseStructDecl(bool should_export, Token name);

    BufferedStream<Token> m_lexer;
};

} // namespace mylang

#endif // MYLANG_SYNTAX_ANALYZER_H
