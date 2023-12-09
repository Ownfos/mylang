#include "file/DummySourceFile.h"
#include "lexer/LexicalAnalyzer.h"
#include "parser/SyntaxAnalyzer.h"
#include "parser/ast/visitor/TreePrinter.h"
#include "parser/ast/visitor/GlobalSymbolScanner.h"
#include "parser/ast/visitor/TypeChecker.h"
#include <iostream>
#include <format>
#include <array>

using namespace mylang;

int main(int argc, char** argv)
{
    try
    {
        auto source_code =
            "module math;\n"
            "foo: func = (a:i32[2][3], b:out i32)->str\n"
            "{\n"
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
            "goo: func = (callback: [(i32, inout str)->bool]){}\n"
            "person: struct =\n"
            "{\n"
            "    name: str;\n"
            "    age: i32;\n"
            "}\n";
        if (argc > 1)
        {
            source_code = argv[1];
        }
        std::cout << "[source code]\n" << source_code << "\n\n";

        // Generate AST from given source code
        auto source_file = std::make_unique<DummySourceFile>(std::move(source_code));
        auto lexer = std::make_unique<LexicalAnalyzer>(std::move(source_file));
        auto syntax_analyzer = SyntaxAnalyzer(std::move(lexer));

        auto ast = syntax_analyzer.GenerateAST();

        // Print AST
        auto printer = TreePrinter(std::cout);
        ast->Accept(&printer);

        // Perform semantic analysis
        auto environment = ProgramEnvironment();
        auto scanner = GlobalSymbolScanner(environment);
        auto type_checker = TypeChecker(environment);

        ast->Accept(&scanner);
        ast->Accept(&type_checker);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}