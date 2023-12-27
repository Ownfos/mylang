#include "file/DummySourceFile.h"
#include "file/DummyOutputFile.h"
#include "file/DummyOutputFileFactory.h"
#include "lexer/LexicalAnalyzer.h"
#include "parser/SyntaxAnalyzer.h"
#include "parser/ast/visitor/GlobalSymbolScanner.h"
#include "codegen/CodeGenerator.h"
#include <gtest/gtest.h>

using namespace mylang;

std::shared_ptr<IAbstractSyntaxTree> GenerateAST(std::string&& source_code)
{
    auto source_file = std::make_unique<DummySourceFile>(std::move(source_code));
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
    for (const auto& ast : ast_list)
    {
        ast->Accept(&scanner);
    }

    auto file_factory = std::make_unique<DummyOutputFileFactory>();
    auto generator = std::make_shared<CodeGenerator>(environment, "", std::move(file_factory));
    
    for (const auto& ast : ast_list)
    {
        ast->Accept(generator.get());
    }

    return generator;
}

void ExpectOutputEquality(IOutputFile* dummy_output_file, std::string_view expected)
{
    // This cast is safe in this test file
    // since we only use DummyOutputFileFactory.
    auto content = dynamic_cast<DummyOutputFile*>(dummy_output_file)->Content();
    ASSERT_EQ(std::string_view(content), expected);
}

TEST(CodeGenerator, EmptyModule)
{
    auto source = "module a;\n";

    auto environment = ProgramEnvironment();
    auto ast_list = std::vector{
        GenerateAST(source)
    };
    auto generator = GenerateOutput(environment, ast_list);
    
    // a.h
    {
        auto expected =
            "#ifndef MODULE_a_H\n"
            "#define MODULE_a_H\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("a.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"a.h\"\n";
        ExpectOutputEquality(generator->GetFile("a.cpp"), expected);
    }
}

TEST(CodeGenerator, SinglePrivateFunction)
{
    // TODO: write test case
    /*
    ```source code
    module a;

    foo: func = () {
        i: i32 = 0;
    }
    ```

    ==>

    ```a.h
    #ifndef MODULE_a_H
    #define MOUDLE_a_H

    #endif MODULE_a_H
    ```

    ```a.cpp
    #include "a.h"

    void foo()
    {
        int i = 0;
    }
    ```
    */
   FAIL();
}