#include "file/DummySourceFile.h"
#include "file/DummyOutputFile.h"
#include "file/SourceFile.h"
#include <gtest/gtest.h>
#include <fstream>

using namespace mylang;

TEST(DummySourceFile, EmptyFile)
{
    auto s = DummySourceFile("");
    ASSERT_EQ(s.GetNext(), '$');
    ASSERT_TRUE(s.IsFinished());
}

TEST(DummySourceFile, SingleLine)
{
    auto s = DummySourceFile("hello, world!");
    for (char ch : std::string("hello, world!"))
    {
        ASSERT_EQ(s.GetNext(), ch);
        ASSERT_FALSE(s.IsFinished());
    }
    ASSERT_EQ(s.GetNext(), (SourceChar{.ch = '$', .pos = SourcePos{.line = 1, .column = 14}}));
    ASSERT_TRUE(s.IsFinished());
}

TEST(DummySourceFile, MultipleLine)
{
    auto s = DummySourceFile("hello\r\nworld!\r\n");
    for (int i = 0; i < 6; i++) s.GetNext();
    ASSERT_EQ(s.GetNext(), (SourceChar{.ch = '\n', .pos = SourcePos{.line = 1, .column = 7}}));
    ASSERT_EQ(s.GetNext(), (SourceChar{.ch = 'w', .pos = SourcePos{.line = 2, .column = 1}}));
}

void CreateTempFile(const std::string& content)
{
    std::ofstream file("temp.txt");
    file << content;
    file.close();
}

void DeleteTempFile()
{
    std::remove("temp.txt");
}

TEST(SourceFile, EmptyFile)
{
    CreateTempFile("");
    {
        auto s = SourceFile("temp.txt");
        ASSERT_EQ(s.GetNext(), '$');
        ASSERT_TRUE(s.IsFinished());
    }
    DeleteTempFile();
}

TEST(SourceFile, SingleLine)
{
    CreateTempFile("ABC DEF");
    {
        auto s = SourceFile("temp.txt");
        for (char ch : std::string("ABC DEF"))
        {
            ASSERT_EQ(s.GetNext(), ch);
            ASSERT_FALSE(s.IsFinished());
        }
        ASSERT_EQ(s.GetNext(), (SourceChar{.ch = '$', .pos = SourcePos{.line = 1, .column = 8}}));
        ASSERT_TRUE(s.IsFinished());
    }
    DeleteTempFile();
}

TEST(SourceFile, MultipleLine)
{
    CreateTempFile("hello\r\nworld!\r\n");
    {
        auto s = SourceFile("temp.txt");
        for (int i = 0; i < 6; i++) s.GetNext();
        ASSERT_EQ(s.GetNext(), (SourceChar{.ch = '\n', .pos = SourcePos{.line = 1, .column = 7}}));
        ASSERT_EQ(s.GetNext(), (SourceChar{.ch = 'w', .pos = SourcePos{.line = 2, .column = 1}}));
    }
    DeleteTempFile();
}

TEST(DummyOutputFile, SingleLine)
{
    auto output = DummyOutputFile();
    output.Open("test.txt");
    output.Print("haha");
    output.Close();

    ASSERT_EQ(output.Content(), "haha");
}

TEST(DummyOutputFile, Indentation)
{
    auto output = DummyOutputFile();
    output.Open("test.txt");
    output.PrintIndented("1\n");
    output.IncreaseDepth();
    output.PrintIndented("2\n");
    output.DecreaseDepth();
    output.PrintIndented("3\n");
    output.Close();

    ASSERT_EQ(output.Content(), "1\n    2\n3\n");
}