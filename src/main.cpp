#include "file/SourceFile.h"
#include "file/OutputFileFactory.h"
#include "lexer/LexicalAnalyzer.h"
#include "parser/SyntaxAnalyzer.h"
#include "parser/ast/visitor/GlobalSymbolScanner.h"
#include "parser/ast/visitor/TypeChecker.h"
#include "parser/ast/visitor/JumpStmtUsageChecker.h"
#include "codegen/CodeGenerator.h"
#include <iostream>

using namespace mylang;

std::shared_ptr<IAbstractSyntaxTree> GenerateAST(const std::filesystem::path& path)
{
    auto source_file = std::make_unique<SourceFile>(path);
    auto lexer = std::make_unique<LexicalAnalyzer>(std::move(source_file));
    auto syntax_analyzer = SyntaxAnalyzer(std::move(lexer));
    return syntax_analyzer.GenerateAST();
}

std::shared_ptr<CodeGenerator> GenerateOutput(
    ProgramEnvironment& environment,
    const std::vector<std::shared_ptr<IAbstractSyntaxTree>>& ast_list
)
{
    auto scanner = GlobalSymbolScanner(environment);
    auto type_checker = TypeChecker(environment);
    auto jump_stmt_checker = JumpStmtUsageChecker();
    for (const auto& ast : ast_list)
    {
        ast->Accept(&scanner);
        ast->Accept(&type_checker);
        ast->Accept(&jump_stmt_checker);
    }

    auto file_factory = std::make_unique<OutputFileFactory>();
    auto generator = std::make_shared<CodeGenerator>(environment, "", std::move(file_factory));
    
    for (const auto& ast : ast_list)
    {
        ast->Accept(generator.get());
    }

    return generator;
}

// Perform semantic analysis and collect global symbol
// information in the given 'environment' instance.
void RunSemanticAnalysis(ProgramEnvironment& environment, IAbstractSyntaxTree* ast)
{
    auto scanner = GlobalSymbolScanner(environment);
    auto type_checker = TypeChecker(environment);
    auto jump_stmt_checker = JumpStmtUsageChecker();

    ast->Accept(&scanner);
    ast->Accept(&type_checker);
    ast->Accept(&jump_stmt_checker);
}

void ValidateArguments(int argc, char** argv)
{
    // We need output directory and at least one input file.
    if (argc <= 1)
    {
        throw std::runtime_error("[Argument Error] more than two arguments are required: [output directory] [input file 1] [input file 2] ... [input file N]");
    }

    // Make sure we can access the output directory.
    try
    {
        std::filesystem::create_directory(argv[0]);
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("[I/O Error] invalid output directory");
    }
}

void PrintUsage()
{
    std::cout << "";
}

int main(int argc, char** argv)
{
    try
    {
        ValidateArguments(argc, argv);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }   
}
