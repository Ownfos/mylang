#ifndef MYLANG_SYNTAX_ANALYZER_H
#define MYLANG_SYNTAX_ANALYZER_H

#include "common/BufferedStream.h"
#include "lexer/Token.h"
#include "parser/ast/IAbstractSyntaxTree.h"
#include "parser/type/Type.h"
#include <memory>
#include <optional>
#include <set>

namespace mylang
{

class Program;
class FuncDecl;
class StructDecl;
class Stmt;
class CompoundStmt;
struct Parameter;

// Generate an abstract syntax tree from given stream of tokens.
class SyntaxAnalyzer
{
public:
    SyntaxAnalyzer(std::unique_ptr<IStream<Token>>&& lexer);

    std::shared_ptr<IAbstractSyntaxTree> GenerateAST();

private:
    // Helper functions for reading tokens of specific type (or types).
    // They all accept current token when types match, but has different behavior on mismatch.
    // - Accept, AcceptOneOf: throws exception
    // - OptionalAccept, OptionalAcceptOneOf: does nothing but return empty std::optional.
    Token Accept(TokenType type);
    Token AcceptOneOf(const std::set<TokenType>& types);
    std::optional<Token> OptionalAccept(TokenType type);
    std::optional<Token> OptionalAcceptOneOf(const std::set<TokenType>& types);

    // Parsing routine for each nonterminals.
    // ParseXXX corresponds to substitution rule XXX -> YYY.
    std::shared_ptr<Program> ParseProgram();
    
    std::shared_ptr<FuncDecl> ParseFuncDecl(bool should_export, Token name);
    std::vector<Parameter> ParseParamList();
    Parameter ParseParam();
    Type ParseType();
    
    std::shared_ptr<StructDecl> ParseStructDecl(bool should_export, Token name);

    std::shared_ptr<Stmt> ParseStmt();
    std::shared_ptr<CompoundStmt> ParseCompoundStmt();

    // The source stream of tokens.
    BufferedStream<Token> m_lexer;
};

} // namespace mylang

#endif // MYLANG_SYNTAX_ANALYZER_H
