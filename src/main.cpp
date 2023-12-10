#include "file/DummySourceFile.h"
#include "lexer/LexicalAnalyzer.h"
#include "parser/SyntaxAnalyzer.h"
#include "parser/ast/visitor/TreePrinter.h"
#include "parser/ast/visitor/GlobalSymbolScanner.h"
#include "parser/ast/visitor/TypeChecker.h"
#include "parser/ast/visitor/JumpStmtUsageChecker.h"
#include <iostream>
#include <format>
#include <array>

using namespace mylang;

// This is the source code used as an input
// when user doesn't provide any input through CLI.
std::string get_demo_source_code()
{
    return
        "module math;\n"
        "\n"
        "foo: func = (a:i32[2][3], b:out i32)->str {\n"
        "    local_var: i32[2][2] = {{1, 2}, {3, 4}};\n"
        "    if (b + a[1][2] > 0)\n"
        "    {\n"
        "        return \"haha\";\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        return \"hoho\";\n"
        "    }\n"
        "}\n"
        "\n"
        "goo: func = (callback: [(i32, inout str)->bool]){}\n"
        "\n"
        "person: struct = {\n"
        "    name: str;\n"
        "    age: i32;\n"
        "}\n";
}

// If user provided CLI argument, use it as input.
// If not, just use the default input data.
std::string get_input_source_code(int argc, char** argv)
{
    if (argc > 1)
    {
        return argv[1];
    }
    else
    {
        return get_demo_source_code();
    }
}

// Perform syntax analysis and return resulting AST.
// This will throw SyntaxError exception when things go wrong.
auto RunSyntaxAnalysis(std::string&& source_code)
{
    auto source_file = std::make_unique<DummySourceFile>(std::move(source_code));
    auto lexer = std::make_unique<LexicalAnalyzer>(std::move(source_file));
    auto syntax_analyzer = SyntaxAnalyzer(std::move(lexer));

    return syntax_analyzer.GenerateAST();
}

void PrintAST(IAbstractSyntaxTree* ast)
{
    auto printer = TreePrinter(std::cout);
    ast->Accept(&printer);
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

int main(int argc, char** argv)
{
    try
    {
        // Choose between default input source code and user input (if any)
        auto source_code = get_input_source_code(argc, argv);
        std::cout << "[Source Code]\n" << source_code << "\n\n";

        // Generate AST from given source code
        std::cout << "[syntax Analysis...]\n";
        auto ast = RunSyntaxAnalysis(std::move(source_code));
        PrintAST(ast.get());

        // Perform semantic analysis
        std::cout << "[Semantic Analysis...]\n";
        auto environment = ProgramEnvironment();
        RunSemanticAnalysis(environment, ast.get());
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }   
}
