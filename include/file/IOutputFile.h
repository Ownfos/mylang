#ifndef MYLANG_I_OUTPUT_FILE_H
#define MYLANG_I_OUTPUT_FILE_H

#include <string>
#include <filesystem>

namespace mylang
{

// An interface for code generation target.
// It could be an actual file or a dummy output which uses std::stringstream.
//
// Every concrete class should close its output stream on destructor,
// if and only if the stream is still open.
class IOutputFile
{
public:
    virtual ~IOutputFile() = default;

    // Begin and end a printing sequence.
    virtual void Open(const std::filesystem::path& path) = 0;
    virtual void Close() = 0;

    // Print a string without indentation or newline.
    virtual void Print(std::string_view msg) = 0;
    
    // Print a string with indentation.
    // Indentation depth can be controlled with
    // IncreaseDepth() and DecreaseDepth().
    inline void PrintIndented(std::string_view msg)
    {
        const auto indentation = std::string(m_indent_level * 4, ' ');
        Print(indentation);
        Print(msg);
    }

    inline void IncreaseDepth()
    {
        ++m_indent_level;
    }

    inline void DecreaseDepth()
    {
        --m_indent_level;
    }

private:
    int m_indent_level = 0;
};

} // namespace mylang


#endif // MYLANG_I_OUTPUT_FILE_H
