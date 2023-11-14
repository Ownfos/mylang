#include "parser/routine/GlobalDeclParser.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"

namespace mylang
{

GlobalDeclParser::GlobalDeclParser(
    std::shared_ptr<BufferedStream<Token>> token_stream,
    std::shared_ptr<IParseRoutine<std::shared_ptr<Stmt>>> stmt_parser,
    std::shared_ptr<IParseRoutine<Type>> type_parser
)
    : IParseRoutine(token_stream)
    , m_stmt_parser(stmt_parser)
    , m_type_parser(type_parser)
{}

bool GlobalDeclParser::CanStartParsing()
{
    return Peek() == TokenType::Export || Peek() == TokenType::Identifier;
}

// global-decl ::= "export"? identifier ":" (func-decl | struct-decl)
std::shared_ptr<GlobalDecl> GlobalDeclParser::Parse()
{
    auto should_export = OptionalAccept(TokenType::Export).has_value();
    auto name = Accept(TokenType::Identifier);
    Accept(TokenType::Colon);

    if (Peek() == TokenType::Func)
    {
        return ParseFuncDecl(should_export, name);
    }
    else
    {
        return ParseStructDecl(should_export, name);
    }
}

// func-decl ::= "func" "=" "(" param-list? ")" ("->" type)? stmt
std::shared_ptr<GlobalDecl> GlobalDeclParser::ParseFuncDecl(bool should_export, const Token& name)
{
    Accept(TokenType::Func);
    Accept(TokenType::Assign);
    Accept(TokenType::LeftParen);
    auto parameters = std::vector<Parameter>{};
    if (Peek() == TokenType::Identifier)
    {
        parameters = ParseParamList();
    }
    Accept(TokenType::RightParen);

    auto return_type = std::optional<Type>{};
    if (OptionalAccept(TokenType::Arrow))
    {
        return_type = m_type_parser->Parse();
    }

    auto body = m_stmt_parser->Parse();

    return std::make_shared<FuncDecl>(
        should_export,
        name,
        return_type,
        parameters,
        body
    );
}

// param-list ::= param ("," param)*
std::vector<Parameter> GlobalDeclParser::ParseParamList()
{
    auto parameters = std::vector<Parameter>{};

    // First parameter comes immediately.
    parameters.push_back(ParseParam());

    // Following paramters have leading comma as a separator.
    while (OptionalAccept(TokenType::Comma))
    {
        parameters.push_back(ParseParam());
    }

    return parameters;
}

// param ::= identifier ":" param-type
Parameter GlobalDeclParser::ParseParam()
{
    auto name = Accept(TokenType::Identifier);
    Accept(TokenType::Colon);

    auto usage = ParseParamUsage();
    auto type = m_type_parser->Parse();

    return Parameter{name, ParamType{type, usage}};
}

ParamUsage GlobalDeclParser::ParseParamUsage()
{
    auto usage = OptionalAcceptOneOf({
        TokenType::In,
        TokenType::Out,
        TokenType::InOut
    });

    // Note: default usage is "in"
    if (!usage || usage->type == TokenType::In)
    {
        return ParamUsage::In;
    }
    else if (usage->type == TokenType::Out)
    {
        return ParamUsage::Out;
    }
    else
    {
        return ParamUsage::InOut;
    }
}

std::shared_ptr<GlobalDecl> GlobalDeclParser::ParseStructDecl(bool should_export, const Token& name)
{
    // TODO: implement
    return {};
}

} // namespace mylang
