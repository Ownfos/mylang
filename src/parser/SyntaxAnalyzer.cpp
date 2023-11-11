#include "parser/SyntaxAnalyzer.h"
#include "parser/ast/Program.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"
#include "parser/type/base/DataType.h"

namespace mylang
{

bool IsFirstOfExpr(TokenType type)
{
    // TODO: return true iff type is in First(expr)
    return false;
}

bool IsFirstOfJumpStmt(TokenType type)
{
    return
        type == TokenType::Return ||
        type == TokenType::Break ||
        type == TokenType::Continue;
}

bool IsFirstOfStmt(TokenType type)
{
    return
        type == TokenType::LeftBrace ||
        type == TokenType::For ||
        type == TokenType::While ||
        type == TokenType::If ||
        IsFirstOfJumpStmt(type) ||
        IsFirstOfExpr(type);
}

bool IsFirstOfParamType(TokenType type)
{
    return
        // Pameter usage (optional prefix)
        type == TokenType::In ||
        type == TokenType::Out ||
        type == TokenType::InOut ||
        // Primitive types
        type == TokenType::IntType ||
        type == TokenType::FloatType ||
        type == TokenType::BoolType ||
        type == TokenType::StringType ||
        // Struct type
        type == TokenType::Identifier ||
        // Function types
        type == TokenType::LeftBracket;
}

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

// program ::= module-decl module-import* global-decl*
// module-decl ::= "module" identifier ";"
// module-import ::= "import" "export"? identifier ";"
// global-decl ::= "export"? identifier ":" (func-decl | struct-decl)
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

// func-decl ::= "func" "=" "(" param-list? ")" ("->" type)? compound-stmt
std::shared_ptr<FuncDecl> SyntaxAnalyzer::ParseFuncDecl(bool should_export, Token name)
{
    Accept(TokenType::Func);
    Accept(TokenType::Equal);
    
    // Parameters.
    Accept(TokenType::LeftParen);
    auto parameters = ParseParamList();
    Accept(TokenType::RightParen);
    
    // Return type.
    auto return_type = std::optional<Type>{};
    if (OptionalAccept(TokenType::Arrow))
    {
        return_type = ParseType();
    }

    // Function body.
    auto body = ParseCompoundStmt();

    return std::make_shared<FuncDecl>(should_export, name, return_type, parameters, body);
}

// param-list ::= param ("," param)*
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

// param ::= identifier ":" param-type
Parameter SyntaxAnalyzer::ParseParam()
{
    auto name = Accept(TokenType::Identifier);
    Accept(TokenType::Colon);

    auto param_type = ParseParamType();

    return Parameter{name, param_type};
}

// param-type ::= param-usage? type
ParamType SyntaxAnalyzer::ParseParamType()
{
    auto usage = ParseParamUsage();
    auto type = ParseType();

    return ParamType{type, usage};
}

ParamUsage SyntaxAnalyzer::ParseParamUsage()
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
        ParamUsage::InOut;
    }
}

// type       ::= base-type array-part*
// array-part ::= "[" int-literal "]"
Type SyntaxAnalyzer::ParseType()
{
    auto base_type = ParseBaseType();

    // ("[" int-literal "]")*
    auto array_sizes = std::vector<int>{};
    while (OptionalAccept(TokenType::LeftBracket))
    {
        auto size = Accept(TokenType::IntLiteral);
        array_sizes.push_back(std::stoi(size.lexeme));
        Accept(TokenType::RightBracket);
    }

    return Type(base_type, array_sizes);
}

std::shared_ptr<IBaseType> SyntaxAnalyzer::ParseBaseType()
{
    // Case 1) primitive types and structs
    auto type = OptionalAcceptOneOf({
        TokenType::IntType,
        TokenType::FloatType,
        TokenType::BoolType,
        TokenType::StringType,
        TokenType::Identifier
    });
    if (type)
    {
        return std::make_shared<DataType>(type.value());
    }
    // Case 2) function type: "[(type 1, type 2, ..., type N) -> type]"
    else if (OptionalAccept(TokenType::LeftBracket))
    {
        // Paramter types: "(type 1, type 2, ..., type N)"
        Accept(TokenType::LeftParen);
        auto param_types = std::vector<ParamType>{};
        if (IsFirstOfParamType(m_lexer.Peek().type))
        {
            // First param type comes immediately.
            param_types.push_back(ParseParamType());

            // Second to last param types come after comma.
            while (OptionalAccept(TokenType::Comma))
            {
                param_types.push_back(ParseParamType());
            }
        }
        Accept(TokenType::RightParen);

        // Return type: "-> type"
        auto return_type = std::optional<Type>{};
        if (OptionalAccept(TokenType::Arrow))
        {
            return_type = ParseType();
        }

        Accept(TokenType::RightBracket);

        return std::make_shared<FuncType>(param_types, return_type);
    }
}

std::shared_ptr<StructDecl> SyntaxAnalyzer::ParseStructDecl(bool should_export, Token name)
{
    // TODO: implement struct parsing
    return {};
}

std::shared_ptr<Stmt> SyntaxAnalyzer::ParseStmt()
{
    // TODO: implement
    if (m_lexer.Peek().type == TokenType::If)
    {
        return ParseIfStmt();
    }
    else if (m_lexer.Peek().type == TokenType::While)
    {
        return ParseWhileStmt();
    }
    else if (m_lexer.Peek().type == TokenType::For)
    {
        return ParseForStmt();
    }
    else if (IsFirstOfJumpStmt(m_lexer.Peek().type))
    {
        return ParseJumpStmt();
    }
    // Since expr and var-decl can start with identifier,
    // we need two lookahead instead of one.
    // 
    // Example)
    // count; // expr
    // count = 0; // expr
    // count: i32 = 0; // var-decl
    else if (m_lexer.Peek(0).type == TokenType::Identifier
        && m_lexer.Peek(1).type == TokenType::Colon)
    {
        return ParseVarDeclStmt();
    }
    else
    {
        return ParseExprStmt();
    }
}

// compound-stmt ::= "{" stmt* "}"
std::shared_ptr<CompoundStmt> SyntaxAnalyzer::ParseCompoundStmt()
{
    Accept(TokenType::LeftBrace);
    auto statements = std::vector<std::shared_ptr<Stmt>>();
    while (IsFirstOfStmt(m_lexer.Peek().type))
    {
        statements.push_back(ParseStmt());
    }
    Accept(TokenType::RightBrace);

    return std::make_shared<CompoundStmt>(statements);
}

std::shared_ptr<IfStmt> SyntaxAnalyzer::ParseIfStmt()
{
    // TODO: implement
    return {};
}

std::shared_ptr<WhileStmt> SyntaxAnalyzer::ParseWhileStmt()
{
    // TODO: implement
    return {};
}

std::shared_ptr<ForStmt> SyntaxAnalyzer::ParseForStmt()
{
    // TODO: implement
    return {};
}

std::shared_ptr<JumpStmt> SyntaxAnalyzer::ParseJumpStmt()
{
    // TODO: implement
    return {};
}

std::shared_ptr<VarDeclStmt> SyntaxAnalyzer::ParseVarDeclStmt()
{
    // TODO: implement
    return {};
}

std::shared_ptr<ExprStmt> SyntaxAnalyzer::ParseExprStmt()
{
    // TODO: implement
    return {};
}

} // namespace mylang
