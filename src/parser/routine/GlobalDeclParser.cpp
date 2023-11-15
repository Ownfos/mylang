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
    try
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
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "global-decl ::= \"export\"? identifier \":\" (func-decl | struct-decl)");
    }
}

// func-decl ::= "func" "=" "(" param-list? ")" ("->" type)? stmt
std::shared_ptr<GlobalDecl> GlobalDeclParser::ParseFuncDecl(bool should_export, const Token& name)
{
    try
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
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "func-decl ::= \"func\" \"=\" \"(\" param-list? \")\" (\"->\" type)? stmt");
    }
}

// param-list ::= param ("," param)*
std::vector<Parameter> GlobalDeclParser::ParseParamList()
{
    try
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
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "param-list ::= param (\",\" param)*");
    }
}

// param ::= identifier ":" param-type
Parameter GlobalDeclParser::ParseParam()
{
    try
    {
        auto name = Accept(TokenType::Identifier);
        Accept(TokenType::Colon);

        auto usage = ParseParamUsage();
        auto type = m_type_parser->Parse();

        return Parameter{name, ParamType{type, usage}};
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "param ::= identifier \":\" param-type");
    }
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

// struct-decl ::= "struct" "=" "{" member-decl* "}"
std::shared_ptr<GlobalDecl> GlobalDeclParser::ParseStructDecl(bool should_export, const Token& name)
{
    try
    {
        Accept(TokenType::Struct);
        Accept(TokenType::Assign);
        Accept(TokenType::LeftBrace);

        // List of member variables.
        auto members = std::vector<MemberVariable>{};
        while (Peek() == TokenType::Identifier)
        {
            members.push_back(ParseMemberDecl());
        }

        Accept(TokenType::RightBrace);

        return std::make_shared<StructDecl>(should_export, name, members);
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "struct-decl ::= \"struct\" \"=\" \"{\" member-decl* \"}\"");
    }
}

// member-decl ::= identifier ":" type ";"
MemberVariable GlobalDeclParser::ParseMemberDecl()
{
    try
    {
        auto name = Accept(TokenType::Identifier);
        Accept(TokenType::Colon);
        auto type = m_type_parser->Parse();
        Accept(TokenType::Semicolon);

        return MemberVariable{name, type};
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "member-decl ::= identifier \":\" type \";\"");
    }
}

} // namespace mylang
