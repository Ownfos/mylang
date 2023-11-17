#include "parser/routine/ModuleParser.h"

namespace mylang
{

ModuleParser::ModuleParser(
    std::shared_ptr<BufferedStream<Token>> token_stream,
    std::shared_ptr<IParseRoutine<std::shared_ptr<GlobalDecl>>> global_decl_parser
)
    : IParseRoutine(token_stream)
    , m_global_decl_parser(global_decl_parser)
{}

bool ModuleParser::CanStartParsing()
{
    return Peek() == TokenType::Module;
}

// program ::= module-decl module-import* global-decl*
std::shared_ptr<Module> ModuleParser::Parse()
{
    try
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

        return std::make_shared<Module>(
            module_name, import_list, global_declarations
        );
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "program ::= module-decl module-import* global-decl*");
    }
}

Token ModuleParser::ParseModuleDecl()
{
    try
    {
        Accept(TokenType::Module);
        auto module_name = Accept(TokenType::Identifier);
        Accept(TokenType::Semicolon);

        return module_name;
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "module-decl ::= \"module\" identifier \";\"");
    }
}

ModuleImportInfo ModuleParser::ParseModuleImport()
{
    try
    {
        Accept(TokenType::Import);
        auto should_export = OptionalAccept(TokenType::Export).has_value();
        auto name = Accept(TokenType::Identifier);
        Accept(TokenType::Semicolon);

        return ModuleImportInfo{should_export, name};
    }
    catch(const ParseRoutineError& e)
    {
        throw PatternMismatchError(e, "module-import ::= \"import\" \"export\"? identifier \";\"");
    }
}

} // namespace mylang
