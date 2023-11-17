#include "parser/SyntaxAnalyzer.h"
#include "parser/routine/ExprParser.h"
#include "parser/routine/TypeParser.h"
#include "parser/routine/StmtParser.h"
#include "parser/routine/GlobalDeclParser.h"

namespace mylang
{

SyntaxAnalyzer::SyntaxAnalyzer(std::unique_ptr<IStream<Token>>&& lexer)
    :m_lexer(std::make_shared<BufferedStream<Token>>(move(lexer)))
{
    // Create program parser from given token stream.
    auto expr_parser = std::make_shared<ExprParser>(m_lexer);
    auto type_parser = std::make_shared<TypeParser>(m_lexer);
    auto stmt_parser = std::make_shared<StmtParser>(m_lexer, expr_parser, type_parser);
    auto global_decl_parser = std::make_shared<GlobalDeclParser>(m_lexer, stmt_parser, type_parser);
    m_parser = std::make_unique<ModuleParser>(m_lexer, global_decl_parser);
}

std::shared_ptr<IAbstractSyntaxTree> SyntaxAnalyzer::GenerateAST()
{
    try
    {
        auto ast = m_parser->Parse();
        if (m_lexer->Peek().type != TokenType::EndOfFile)
        {
            throw LeftoverTokenError(m_lexer->GetNext());
        }
        return ast;
    }
    catch(const ParseRoutineError& e)
    {
        throw SyntaxError(e.Location(), e.Description());
    }
}

} // namespace mylang
