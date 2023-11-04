#include "file/DummySourceFile.h"
#include "common/BufferedStream.h"
#include <gtest/gtest.h>

using namespace mylang;

TEST(BufferedStream, SingleLine)
{
    auto source = std::make_unique<DummySourceFile>("hello, world!");
    auto stream = BufferedStream<SourceChar>(std::move(source));
    for (auto ch : std::string("hello, world!"))
    {
        ASSERT_EQ(stream.GetNext(), ch);
        ASSERT_FALSE(stream.IsFinished());
    }
    ASSERT_EQ(stream.GetNext(), (SourceChar{.ch = '$', .pos = SourcePos{.line = 1, .column = 14}}));
    ASSERT_TRUE(stream.IsFinished());
}

TEST(BufferedStream, Peek)
{
    auto source = std::make_unique<DummySourceFile>("hello, world!");
    auto stream = BufferedStream<SourceChar>(std::move(source));

    auto expected = std::string("hello, world!$$$");
    for (unsigned int offset = 0; offset < expected.size(); ++offset)
    {
        ASSERT_EQ(stream.Peek(offset), expected[offset]);
        ASSERT_FALSE(stream.IsFinished());
    }

    for (unsigned int offset = 0; offset < expected.size(); ++offset)
    {
        ASSERT_EQ(stream.GetNext(), expected[offset]);
    }
    ASSERT_TRUE(stream.IsFinished());
}

TEST(BufferedStream, AcceptDiscard)
{
    auto source = std::make_unique<DummySourceFile>("AaBbCcDd");
    auto stream = BufferedStream<SourceChar>(std::move(source));

    // Store characters with odd index.
    for (int i = 0; i < 4; ++i)
    {
        stream.Accept();
        stream.Discard();
    }
    ASSERT_EQ(stream.GetNext(), (SourceChar{.ch = '$', .pos = SourcePos{.line = 1, .column = 9}}));
    ASSERT_TRUE(stream.IsFinished());

    // Check if accept history matches expectation.
    auto expected = std::string("ABCD");
    auto accept_history = stream.GetAcceptHistory();

    ASSERT_EQ(accept_history.size(), expected.size());
    for (int i = 0; i < accept_history.size(); ++i)
    {
        ASSERT_EQ(accept_history[i].ch, expected[i]);
    }
}

TEST(BufferedStream, Rewind)
{
    auto source = std::make_unique<DummySourceFile>("AaBbCcDd");
    auto stream = BufferedStream<SourceChar>(std::move(source));

    // Store characters with odd index.
    for (int i = 0; i < 4; ++i)
    {
        stream.Accept();
        stream.Discard();
    }
    ASSERT_EQ(stream.GetNext(), (SourceChar{.ch = '$', .pos = SourcePos{.line = 1, .column = 9}}));
    ASSERT_TRUE(stream.IsFinished());

    // Is accept history reverted?
    stream.Rewind();
    for (auto ch : std::string("ABCD"))
    {
        ASSERT_EQ(stream.GetNext().ch, ch);
    }
    ASSERT_EQ(stream.GetNext(), (SourceChar{.ch = '$', .pos = SourcePos{.line = 1, .column = 9}}));
    ASSERT_TRUE(stream.IsFinished());
}

TEST(BufferedStream, RewindCheckpoint)
{
    auto source = std::make_unique<DummySourceFile>("1234");
    auto stream = BufferedStream<SourceChar>(std::move(source));

    stream.Accept();
    stream.Accept();
    stream.MarkRewindCheckpoint();
    stream.Accept();
    stream.Accept();

    stream.Rewind();
    auto accept_history = stream.GetAcceptHistory();
    ASSERT_EQ(accept_history.size(), 2);
    ASSERT_EQ(accept_history[0], '1');
    ASSERT_EQ(accept_history[1], '2');

    ASSERT_EQ(stream.GetNext().ch, '3');
    ASSERT_EQ(stream.GetNext().ch, '4');
    ASSERT_EQ(stream.GetNext().ch, '$');
    ASSERT_TRUE(stream.IsFinished());
}