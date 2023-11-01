#include "lexer/LexicalAnalyzer.h"
#include "file/DummySourceFile.h"
#include <iostream>
#include <format>

using namespace mylang;

int main(int argc, char** argv)
{
    try
    {
        auto source_file = std::make_unique<DummySourceFile>(argv[1]);
        auto lexer = LexicalAnalyzer(std::move(source_file));
        while(true)
        {
            auto token = lexer.GetNextToken();

            std::cout << std::format("type: {}, lexeme: {}, line: {}, col: {} ~ {}\n\n",
                static_cast<int>(token.type),
                token.lexeme,
                token.start_pos.line,
                token.start_pos.column,
                token.end_pos.column
            );

            if (token.type == TokenType::EndOfFile) break;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}