#include "lexer/LookAheadManager.h"
#include <numeric>

namespace mylang
{

LookAheadManager::LookAheadManager(std::unique_ptr<ISourceFile>&& source_file)
    : m_source_file(std::move(source_file))
{
    m_lookahead_buffer.push(LookAheadData{
        .ch = m_source_file->CurrentChar(),
        .pos = m_source_file->CurrentPos()
    });
}

bool LookAheadManager::IsEOF() const
{
    // Returns true iff when we have EOF ('$') stored in the buffer and source file is depleted.
    return m_lookahead_buffer.size() == 1 && m_source_file->IsEOF();
}

char LookAheadManager::Peek() const
{
    return m_lookahead_buffer.top().ch;
}

void LookAheadManager::Accept()
{
    // Add current lookahead to the buffer.
    m_lexeme_buffer.push_back(m_lookahead_buffer.top());

    // Remove current lookahead and read next one if needed.
    Discard();
}

void LookAheadManager::Discard()
{
    // Do nothing if we are at EOF.
    if (IsEOF()) return;

    // Ignore current lookahead.
    m_lookahead_buffer.pop();

    // Fetch next character.
    if (m_lookahead_buffer.empty())
    {
        m_source_file->ReadNext();
        m_lookahead_buffer.push(LookAheadData{
            .ch = m_source_file->CurrentChar(),
            .pos = m_source_file->CurrentPos()
        });
    }
}

void LookAheadManager::MarkRewindCheckpoint()
{
    m_rewind_checkpoint = static_cast<int>(m_lexeme_buffer.size());
}

void LookAheadManager::RewindUntilCheckpoint()
{
    for (int i = static_cast<int>(m_lexeme_buffer.size()) - 1; i >= m_rewind_checkpoint; --i)
    {
        m_lookahead_buffer.push(m_lexeme_buffer.back());
        m_lexeme_buffer.pop_back();
    }
}

Token LookAheadManager::CreateToken(TokenType type)
{
    // Accumulate lexeme string.
    auto lexeme = std::string("");
    for (auto [ch, pos] : m_lexeme_buffer)
    {
        lexeme.push_back(ch);
    }

    // Create token.
    auto token = Token{
        .type = type,
        .lexeme = lexeme,
        .start_pos = m_lexeme_buffer.front().pos,
        .end_pos = m_lexeme_buffer.back().pos,
    };

    // Discard used lexeme and reset.
    CleanUpLexemeBuffer();

    return token;
}

void LookAheadManager::CleanUpLexemeBuffer()
{
    m_lexeme_buffer.clear();
    m_rewind_checkpoint = 0;
}

} // namespace mylang
