#ifndef MYLANG_MODULE_PARSER_H
#define MYLANG_MODULE_PARSER_H

#include "parser/routine/IParseRoutine.h"
#include "parser/ast/Module.h"
#include "parser/ast/globdecl/GlobalDecl.h"

namespace mylang
{

class ModuleParser : public IParseRoutine<std::shared_ptr<Module>>
{
public:
    ModuleParser(
        std::shared_ptr<BufferedStream<Token>> token_stream,
        std::shared_ptr<IParseRoutine<std::shared_ptr<GlobalDecl>>> global_decl_parser
    );

    virtual bool CanStartParsing() override;
    virtual std::shared_ptr<Module> Parse() override;

private:
    Token ParseModuleDecl();
    ModuleImportInfo ParseModuleImport();

    std::shared_ptr<IParseRoutine<std::shared_ptr<GlobalDecl>>> m_global_decl_parser;
};

} // namespace mylang

#endif // MYLANG_MODULE_PARSER_H
