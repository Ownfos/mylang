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
            "#include <functional>\n"
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
    auto source =
        "module a;\n"
        "foo: func = (){\n"
        "    i: i32 = 0;\n"
        "}\n";

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
            "#include <functional>\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("a.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"a.h\"\n"
            "void foo();\n"
            "void foo() {\n"
            "    int i = 0;\n"
            "}\n";
        ExpectOutputEquality(generator->GetFile("a.cpp"), expected);
    }
}

TEST(CodeGenerator, SinglePrivateStruct)
{
    auto source =
        "module a;\n"
        "foo: struct = {\n"
        "    i: i32;\n"
        "    j: str;\n"
        "}\n";

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
            "#include <functional>\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("a.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"a.h\"\n"
            "struct foo {\n"
            "    int i;\n"
            "    std::string j;\n"
            "};\n";
        ExpectOutputEquality(generator->GetFile("a.cpp"), expected);
    }
}

TEST(CodeGenerator, SinglePublicStruct)
{
    auto source =
        "module a;\n"
        "export foo: struct = {\n"
        "    i: i32;\n"
        "    j: str;\n"
        "}\n";

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
            "#include <functional>\n"
            "struct foo {\n"
            "    int i;\n"
            "    std::string j;\n"
            "};\n"
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

TEST(CodeGenerator, ArrayVariableNoInitializer)
{
    auto source =
        "module a;\n"
        "foo: func = (){\n"
        "    i: i32[2][3];\n"
        "}\n";

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
            "#include <functional>\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("a.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"a.h\"\n"
            "void foo();\n"
            "void foo() {\n"
            "    std::array<std::array<int, 3>, 2> i;\n"
            "}\n";
        ExpectOutputEquality(generator->GetFile("a.cpp"), expected);
    }
}

TEST(CodeGenerator, ArrayVariableNestedInitializer)
{
    auto source =
        "module a;\n"
        "foo: func = (){\n"
        "    i: i32[2][3] = {{1}, {2, 3}};\n"
        "}\n";

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
            "#include <functional>\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("a.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"a.h\"\n"
            "void foo();\n"
            "void foo() {\n"
            "    std::array<std::array<int, 3>, 2> i = {{{1}, {2, 3}}};\n"
            "}\n";
        ExpectOutputEquality(generator->GetFile("a.cpp"), expected);
    }
}

TEST(CodeGenerator, ExprStmt)
{
    auto source =
        "module a;\n"
        "foo: func = (){\n"
        "    1+2*3==4 && false;\n"
        "}\n";

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
            "#include <functional>\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("a.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"a.h\"\n"
            "void foo();\n"
            "void foo() {\n"
            "    (((1 + (2 * 3)) == 4) && false);\n"
            "}\n";
        ExpectOutputEquality(generator->GetFile("a.cpp"), expected);
    }
}

TEST(CodeGenerator, IfStmt)
{
    auto source =
        "module a;\n"
        "foo: func = (){\n"
        "    if (true) {\n"
        "        name: str = \"mylang\";\n"
        "    }\n"
        "}\n";

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
            "#include <functional>\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("a.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"a.h\"\n"
            "void foo();\n"
            "void foo() {\n"
            "    if (true) {\n"
            "        std::string name = \"mylang\";\n"
            "    }\n"
            "}\n";
        ExpectOutputEquality(generator->GetFile("a.cpp"), expected);
    }
}

TEST(CodeGenerator, ForStmt)
{
    auto source =
        "module a;\n"
        "foo: func = (){\n"
        "    for (i: i32 = 0; i < 10; ++i) {\n"
        "        name: str = \"mylang\";\n"
        "    }\n"
        "}\n";

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
            "#include <functional>\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("a.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"a.h\"\n"
            "void foo();\n"
            "void foo() {\n"
            "    {\n"
            "        int i = 0;\n"
            "        while (true) {\n"
            "            if ((i < 10) == false) break;\n"
            "            {\n"
            "                std::string name = \"mylang\";\n"
            "            }\n"
            "            (++i);\n"
            "        }\n"
            "    }\n"
            "}\n";
        ExpectOutputEquality(generator->GetFile("a.cpp"), expected);
    }
}

TEST(CodeGenerator, JumpStmt)
{
    auto source =
        "module a;\n"
        "foo: func = () -> i32 {\n"
        "    while (true) {\n"
        "        break;\n"
        "        continue;\n"
        "    }\n"
        "    return 0;\n"
        "}\n";

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
            "#include <functional>\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("a.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"a.h\"\n"
            "int foo();\n"
            "int foo() {\n"
            "    while (true) {\n"
            "        break;\n"
            "        continue;\n"
            "    }\n"
            "    return 0;\n"
            "}\n";
        ExpectOutputEquality(generator->GetFile("a.cpp"), expected);
    }
}

TEST(CodeGenerator, TwoModuleImplmentationFile)
{
    auto source1 =
        "module math;\n"
        "add: func = (a: i32, b: i32) -> i32 {\n"
        "    return a + b;\n"
        "}\n";
    auto source2 =
        "module a;\n"
        "squared_add: func = (a: i32, b: i32) -> i32 {\n"
        "    result: i32 = add(a, b);\n"
        "    return result * result;\n"
        "}\n";

    auto environment = ProgramEnvironment();
    auto ast_list = std::vector{
        GenerateAST(source1),
        GenerateAST(source2)
    };
    auto generator = GenerateOutput(environment, ast_list);
    
    // math.h
    {
        auto expected =
            "#ifndef MODULE_math_H\n"
            "#define MODULE_math_H\n"
            "#include <functional>\n"
            "#endif // MODULE_a_H\n";
        ExpectOutputEquality(generator->GetFile("math.h"), expected);
    }
    // a.cpp
    {
        auto expected =
            "#include \"math.h\"\n"
            "int add(const int& a, const int& b);\n"
            "int squared_add(const int& a, const int& b);"
            "int add(const int& a, const int& b) {\n"
            "    return (a + b);\n"
            "}\n"
            "int squared_add(const int& a, const int& b) {\n"
            "    int result = add(a, b);\n"
            "    return (result * result);\n"
            "}\n";
        ExpectOutputEquality(generator->GetFile("math.cpp"), expected);
    }
}