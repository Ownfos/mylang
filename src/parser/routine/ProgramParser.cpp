#include "parser/routine/ProgramParser.h"

namespace mylang
{

ProgramParser::ProgramParser(
    std::shared_ptr<BufferedStream<Token>> token_stream,
    std::shared_ptr<IParseRoutine<std::shared_ptr<GlobalDecl>>> global_decl_parser
)
    : IParseRoutine(token_stream)
    , m_global_decl_parser(global_decl_parser)
{}

bool ProgramParser::CanStartParsing()
{
    return Peek() == TokenType::Module;
}

// program ::= module-decl module-import* global-decl*
std::shared_ptr<Program> ProgramParser::Parse()
{
    auto module_name = ParseModuleDecl();

    auto import_list = std::vector<ModuleImportInfo>{};
    while (Peek() == TokenType::Import)
    {
        import_list.push_back(ParseModuleImport());
    }

    auto global_declarations = std::vector<std::shared_ptr<GlobalDecl>>{};
    while (m_global_decl_parser->CanStartParsing())
    {
        global_declarations.push_back(m_global_decl_parser->Parse());
    }

    return std::make_shared<Program>(
        module_name, import_list, global_declarations);
}

Token ProgramParser::ParseModuleDecl()
{
    Accept(TokenType::Module);
    auto module_name = Accept(TokenType::Identifier);
    Accept(TokenType::Semicolon);

    return module_name;
}

ModuleImportInfo ProgramParser::ParseModuleImport()
{
    Accept(TokenType::Import);
    auto should_export = OptionalAccept(TokenType::Export).has_value();
    auto name = Accept(TokenType::Identifier);
    Accept(TokenType::Semicolon);

    return ModuleImportInfo{should_export, name};
}

} // namespace mylang
