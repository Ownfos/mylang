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
        auto source_code = std::string("module math; import a; import export b; foo: func = (a:i32, b:out j32) goo: func = ()->str");
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