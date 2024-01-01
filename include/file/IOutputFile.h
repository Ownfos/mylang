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
    //
    // If DisableNextIndentation() was called right before,
    // this output will not be indented.
    inline void PrintIndented(std::string_view msg)
    {
        if (m_is_next_indent_disabled)
        {
            m_is_next_indent_disabled = false;
        }
        else
        {
            const auto indentation = std::string(m_indent_level * 4, ' ');
            Print(indentation);
        }
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

    // This is used to generate C++ codes like if-else if statements without line separation.
    //
    // example) "if (...) {} else if (...) {}"
    // To generate a code for the input source code given above,
    // we need to print "else if ..." on the same line.
    // But on the other hand, the first "if" must have proper indentation.
    //
    // We can achieve this by using PrintIndented() while visiting IfStmt
    // and temporarily calling DisableNextIndentation() right after we print "else ".
    inline void DisableNextIndentation()
    {
        m_is_next_indent_disabled = true;
    }

private:
    int m_indent_level = 0;
    bool m_is_next_indent_disabled = false;
};

} // namespace mylang


#endif // MYLANG_I_OUTPUT_FILE_H
