#ifndef MYLANG_GLOBAL_DECL_PARSER_H
#define MYLANG_GLOBAL_DECL_PARSER_H

#include "parser/routine/IParseRoutine.h"
#include "parser/ast/globdecl/GlobalDecl.h"
#include "parser/ast/stmt/Stmt.h"
#include "parser/type/Type.h"

namespace mylang
{

struct Parameter;
enum class ParamUsage;

class GlobalDeclParser : public IParseRoutine<std::shared_ptr<GlobalDecl>>
{
public:
    GlobalDeclParser(
        std::shared_ptr<BufferedStream<Token>> token_stream,
        std::shared_ptr<IParseRoutine<std::shared_ptr<Stmt>>> stmt_parser,
        std::shared_ptr<IParseRoutine<Type>> type_parser
    );

    virtual bool CanStartParsing() override;
    virtual std::shared_ptr<GlobalDecl> Parse() override;

private:
    std::shared_ptr<GlobalDecl> ParseFuncDecl(bool should_export, const Token& name);
    std::shared_ptr<GlobalDecl> ParseStructDecl(bool should_export, const Token& name);

    std::vector<Parameter> ParseParamList();
    Parameter ParseParam();
    ParamUsage ParseParamUsage();

    std::shared_ptr<IParseRoutine<std::shared_ptr<Stmt>>> m_stmt_parser;
    std::shared_ptr<IParseRoutine<Type>> m_type_parser;
};

} // namespace mylang

#endif // MYLANG_GLOBAL_DECL_PARSER_H
