#include "lexer/LexicalAnalyzer.h"
#include "file/DummySourceFile.h"
#include <iostream>

using namespace mylang;

int main(int argc, char** argv)
{
    try
    {
        auto source_file = std::make_unique<DummySourceFile>("/*adf*/fff\r\n/*dfffff\r\n");
        auto lexer = LexicalAnalyzer(std::move(source_file));
        lexer.GetNextToken();
        lexer.GetNextToken();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}