#include "file/SourceFile.h"
#include <iostream>

using namespace mylang;

int main(int argc, char** argv)
{
    try
    {
        auto s = SourceFile(argv[1]);
        while (!s.IsEOF())
        {
            std::cout << s.CurrentChar();
            s.ReadNext();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}