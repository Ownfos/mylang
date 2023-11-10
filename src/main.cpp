#include "file/DummySourceFile.h"
#include "lexer/LexicalAnalyzer.h"
#include "parser/SyntaxAnalyzer.h"
#include "parser/ast/visitor/TreePrinter.h"
#include <iostream>
#include <format>

using namespace mylang;

int main(int argc, char** argv)
{
    try
    {
        auto source_code = std::string("module math;\nimport a;\nimport export b;\nfoo: func = (a:i32[2][3], b:out i32)->str{}\ngoo: func = (callback: [(i32, inout str)->bool]){}");
        if (argc > 1)
        {
            source_code = std::string(argv[1]);
        }
        std::cout << "[source code]\n" << source_code << "\n\n";

        auto source_file = std::make_unique<DummySourceFile>(std::move(source_code));
        auto lexer = std::make_unique<LexicalAnalyzer>(std::move(source_file));
        auto syntax = std::make_unique<SyntaxAnalyzer>(std::move(lexer));

        auto ast = syntax->GenerateAST();

        auto printer = TreePrinter(std::cout);
        ast->Accept(&printer);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}