#ifndef MYLANG_TYPE_PARSER_H
#define MYLANG_TYPE_PARSER_H

#include "parser/routine/IParseRoutine.h"
#include "parser/type/Type.h"

namespace mylang
{

struct ParamType;
enum class ParamUsage;

class TypeParser : public IParseRoutine<Type>
{
public:
    TypeParser(std::shared_ptr<BufferedStream<Token>> token_stream);

    virtual bool CanStartParsing() override;
    virtual Type Parse() override;

private:
    std::shared_ptr<IBaseType> ParseBaseType();
    ParamType ParseParamType();
    std::optional<ParamUsage> TryParseParamUsage();
};

} // namespace mylang

#endif // MYLANG_TYPE_PARSER_H
