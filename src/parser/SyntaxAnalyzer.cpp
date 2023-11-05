#include "parser/SyntaxAnalyzer.h"
#include "parser/ast/Program.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"

namespace mylang
{

SyntaxAnalyzer::SyntaxAnalyzer(std::unique_ptr<IStream<Token>>&& lexer)
    : m_lexer(std::move(lexer))
{}

std::shared_ptr<IAbstractSyntaxTree> SyntaxAnalyzer::GenerateAST()
{
    auto program = ParseProgram();
    if (m_lexer.GetNext().type != TokenType::EndOfFile)
    {
        // TODO: throw leftover token exception
    }
    return program;
}

Token SyntaxAnalyzer::Accept(TokenType type)
{
    auto token = m_lexer.GetNext();
    if (token.type != type)
    {
        // TODO: throw unexpected token exception
    }
    return token;
}

Token SyntaxAnalyzer::AcceptOneOf(const std::set<TokenType>& types)
{
    auto token = m_lexer.GetNext();
    if (types.count(token.type) == 0)
    {
        // TODO: throw unexpected token exception
    }
    return token;
}

std::optional<Token> SyntaxAnalyzer::OptionalAccept(TokenType type)
{
    if (m_lexer.Peek().type == type)
    {
        return m_lexer.GetNext();
    }
    else
    {
        return {};
    }
}

std::optional<Token> SyntaxAnalyzer::OptionalAcceptOneOf(const std::set<TokenType>& types)
{
    if (types.count(m_lexer.Peek().type) != 0)
    {
        return m_lexer.GetNext();
    }
    else
    {
        return {};
    }
}

std::shared_ptr<Program> SyntaxAnalyzer::ParseProgram()
{
    // Module declaration.
    Accept(TokenType::Module);
    auto module_name = Accept(TokenType::Identifier);
    Accept(TokenType::Semicolon);

    // Module import directives.
    std::vector<ModuleImportInfo> import_list;
    while(OptionalAccept(TokenType::Import))
    {
        auto export_token = OptionalAccept(TokenType::Export);
        auto name_token = Accept(TokenType::Identifier);
        Accept(TokenType::Semicolon);
        
        // Record import information.
        import_list.push_back(ModuleImportInfo{
            .should_export = export_token.has_value(),
            .name = name_token
        });
    }

    // Function and struct delclarations.
    std::vector<std::shared_ptr<GlobalDecl>> global_declarations;
    while (m_lexer.Peek().type == TokenType::Export || m_lexer.Peek().type == TokenType::Identifier)
    {
        auto should_export = OptionalAccept(TokenType::Export);
        auto name = Accept(TokenType::Identifier);
        Accept(TokenType::Colon);

        if (m_lexer.Peek().type == TokenType::Func)
        {
            global_declarations.push_back(ParseFuncDecl(should_export.has_value(), name));
        }
        else if (m_lexer.Peek().type == TokenType::Struct)
        {
            global_declarations.push_back(ParseFuncDecl(should_export.has_value(), name));
        }
        else
        {
            // TODO: throw exception (invalid global declaration)
        }
    }

    return std::make_shared<Program>(module_name, import_list, global_declarations);
}

std::shared_ptr<FuncDecl> SyntaxAnalyzer::ParseFuncDecl(bool should_export, Token name)
{
    Accept(TokenType::Func);
    Accept(TokenType::Equal);
    
    // Parameters.
    Accept(TokenType::LeftParen);
    auto parameters = ParseParamList();
    Accept(TokenType::RightParen);
    
    // Return type.
    auto return_type = std::optional<Token>{};
    if (OptionalAccept(TokenType::Arrow))
    {
        return_type = ParseType();
    }

    // Function body.
    // TODO: add CompoundStmt parsing when ready.

    return std::make_shared<FuncDecl>(should_export, name, return_type, parameters/*, body*/);
}

std::vector<Parameter> SyntaxAnalyzer::ParseParamList()
{
    std::vector<Parameter> parameters;

    if (m_lexer.Peek().type == TokenType::Identifier)
    {
        // First parameter comes immediately.
        parameters.push_back(ParseParam());

        // Following paramters have leading comma as a separator.
        while (OptionalAccept(TokenType::Comma))
        {
            parameters.push_back(ParseParam());
        }
    }

    return parameters;
}

Parameter SyntaxAnalyzer::ParseParam()
{
    auto name = Accept(TokenType::Identifier);
    Accept(TokenType::Colon);

    // Optional usage specification where default is "in".
    auto usage = OptionalAcceptOneOf({
        TokenType::In,
        TokenType::Out,
        TokenType::InOut
    });

    auto type = ParseType();

    return Parameter{name, usage, type};
}

Token SyntaxAnalyzer::ParseType()
{
    return AcceptOneOf({
        TokenType::Identifier,
        TokenType::IntType,
        TokenType::FloatType,
        TokenType::BoolType,
        TokenType::StringType
    });
}

std::shared_ptr<StructDecl> SyntaxAnalyzer::ParseStructDecl(bool should_export, Token name)
{
    // TODO: implement struct parsing
    return {};
}

} // namespace mylang
