#ifndef MYLANG_PROGRAM_PARSER_H
#define MYLANG_PROGRAM_PARSER_H

#include "parser/routine/IParseRoutine.h"
#include "parser/ast/Program.h"
#include "parser/ast/globdecl/GlobalDecl.h"

namespace mylang
{

class ProgramParser : public IParseRoutine<std::shared_ptr<Program>>
{
public:
    ProgramParser(
        std::shared_ptr<BufferedStream<Token>> token_stream,
        std::shared_ptr<IParseRoutine<std::shared_ptr<GlobalDecl>>> global_decl_parser
    );

    virtual bool CanStartParsing() override;
    virtual std::shared_ptr<Program> Parse() override;

private:
    Token ParseModuleDecl();
    ModuleImportInfo ParseModuleImport();

    std::shared_ptr<IParseRoutine<std::shared_ptr<GlobalDecl>>> m_global_decl_parser;
};

} // namespace mylang

#endif // MYLANG_PROGRAM_PARSER_H
