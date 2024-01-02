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

struct CommandLineArguments
{
    std::filesystem::path output_directory;
    std::vector<std::filesystem::path> input_file_paths;
};

auto ParseCommandLine(int argc, char** argv)
{
    // We need output directory and at least one input file.
    // Note: since the executable's name is treated as the first argument,
    //       we should validate that argc is greater than 2.
    if (argc <= 2)
    {
        throw std::exception("[Argument Error] more than two arguments are required: [output directory] [input file 1] [input file 2] ... [input file N]");
    }

    // Store arguments as path.
    auto arguments = CommandLineArguments{};
    arguments.output_directory = argv[1];
    for (int i = 2; i < argc; ++i)
    {
        arguments.input_file_paths.push_back(argv[i]);
    }

    // Make sure we can access the output directory.
    try
    {
        std::filesystem::create_directory(arguments.output_directory);
    }
    catch(...)
    {
        throw std::exception("[I/O Error] invalid output directory");
    }

    return arguments;
}

// Generates an AST for a given input file.
// An exception will be thrown for any lexical or syntactic error.
std::shared_ptr<IAbstractSyntaxTree> RunLexicalAndSyntaxAnalysis(const std::filesystem::path& input_file_path)
{
    auto source_file = std::make_unique<SourceFile>(input_file_path);
    auto lexer = std::make_unique<LexicalAnalyzer>(std::move(source_file));
    auto syntax_analyzer = SyntaxAnalyzer(std::move(lexer));

    return syntax_analyzer.GenerateAST();
}

// Perform semantic analysis and collect global symbol
// information in the given 'environment' instance.
// An exception will be thrown for any semantic error.
void RunSemanticAnalysis(ProgramEnvironment& environment, IAbstractSyntaxTree* ast)
{
    auto scanner = GlobalSymbolScanner(environment);
    auto type_checker = TypeChecker(environment);
    auto jump_stmt_checker = JumpStmtUsageChecker();

    ast->Accept(&scanner);
    ast->Accept(&type_checker);
    ast->Accept(&jump_stmt_checker);
}

void PrintFrontendError(const std::filesystem::path& input_file_path)
{
    std::cerr << std::format("# Error occured while parsing file \'{}\'\n", input_file_path.string());
}

std::vector<std::shared_ptr<IAbstractSyntaxTree>> RunCompilerFrontend(
    const std::vector<std::filesystem::path>& input_file_paths,
    ProgramEnvironment& environment
)
{
    // Step 1) generate AST for each input source file.
    auto ast_list = std::vector<std::shared_ptr<IAbstractSyntaxTree>>();
    for (const auto& input_file_path : input_file_paths)
    {
        try
        {
            ast_list.push_back(RunLexicalAndSyntaxAnalysis(input_file_path));
        }
        catch(...)
        {
            PrintFrontendError(input_file_path);
            throw;
        }
    }

    // Step 2) scan import directives and global symbols.
    auto scanner = GlobalSymbolScanner(environment);
    for (int i = 0; i < ast_list.size(); ++i)
    {
        try
        {
            ast_list[i]->Accept(&scanner);
        }
        catch(...)
        {
            PrintFrontendError(input_file_paths[i]);
            throw;
        }
    }

    // Step 3) run semantic checks.
    auto type_checker = TypeChecker(environment);
    auto jump_stmt_checker = JumpStmtUsageChecker();
    for (int i = 0; i < ast_list.size(); ++i)
    {
        try
        {
            ast_list[i]->Accept(&type_checker);
            ast_list[i]->Accept(&jump_stmt_checker);
        }
        catch(...)
        {
            PrintFrontendError(input_file_paths[i]);
            throw;
        }
    }

    return ast_list;
}

void RunCompilerBackend(
    const std::filesystem::path& output_directory,
    const std::vector<std::shared_ptr<IAbstractSyntaxTree>>& ast_list,
    ProgramEnvironment& environment
)
{
    auto file_factory = std::make_unique<OutputFileFactory>();
    auto generator = std::make_shared<CodeGenerator>(environment, output_directory, std::move(file_factory));
    
    for (const auto& ast : ast_list)
    {
        ast->Accept(generator.get());
    }
}

int main(int argc, char** argv)
{
    try
    {
        // Get the output directory and list of input file paths.
        auto arguments = ParseCommandLine(argc, argv);

        // Read and analyze the input files to generate AST.
        // This step involves lexical analyzer, syntax analyzer, and semantic analyzer.
        auto environment = ProgramEnvironment();
        auto ast_list = RunCompilerFrontend(arguments.input_file_paths, environment);

        // Generate C++ code for given ASTs.
        // One header file and one source file will be created for each logical module.
        RunCompilerBackend(arguments.output_directory, ast_list, environment);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }   
}
