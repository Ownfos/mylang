#include "codegen/CodeGenerator.h"
#include "parser/ast/Module.h"

namespace mylang
{

CodeGenerator::CodeGenerator(
    ProgramEnvironment& environment,
    const std::filesystem::path& output_dir,
    std::unique_ptr<IOutputFileFactory>&& file_factory
)
    : m_environment(environment)
    , m_output_dir(output_dir)
    , m_file_factory(std::move(file_factory))
{}

CodeGenerator::~CodeGenerator()
{
    CloseAllFiles();
}

IOutputFile* CodeGenerator::GetFile(const std::string& file_name)
{
    auto it = m_open_files.find(file_name);

    // If we have an open file, return it.
    if (it != m_open_files.end())
    {
        return it->second.get();
    }
    // Create new one if it doesn't exist.
    else
    {
        auto new_file = m_file_factory->CreateOutputFile();
        new_file->Open(m_output_dir / file_name);
        m_open_files.emplace(file_name, new_file);

        return new_file.get();
    }
}

void CodeGenerator::CloseAllFiles()
{
    for (auto& [name, file] : m_open_files)
    {
        file->Close();
    }
}

bool CodeGenerator::IsModuleNodeVisited(const std::string& module_name) const
{
    return m_visited_modules.count(module_name) > 0;
}

void CodeGenerator::MarkModuleAsVisited(const std::string& module_name)
{
    m_visited_modules.insert(module_name);
}

std::string HeaderGuardMacro(const std::string& module_name)
{
    return std::format("MODULE_{}_H", module_name);
}

std::string HeaderFileName(const std::string& module_name)
{
    return std::format("{}.h", module_name);
}

std::string IncludeHeaderMacro(const std::string& module_name)
{
    return std::format("#include \"{}\"\n", HeaderFileName(module_name));
}

void CodeGenerator::InitializeHeaderFile(const std::string& module_name)
{
    auto header_file = GetFile(HeaderFileName(module_name));

    // Open header guard
    auto header_guard = HeaderGuardMacro(module_name);
    header_file->Print(std::format("#ifndef {}\n", header_guard));
    header_file->Print(std::format("#define {}\n", header_guard));

    // Emit global module import directives
    const auto& module_info = m_environment.GetModuleInfo(module_name);
    for (const auto& import_directive : module_info.import_list)
    {
        if (import_directive.should_export)
        {
            header_file->Print(IncludeHeaderMacro(import_directive.name.lexeme));
        }
    }

    // TODO: emit global symbol forward declarations

    // Close header guard
    header_file->Print(std::format("#endif // {}\n", header_guard));
}

void CodeGenerator::InitializeSourceFile(const std::string& module_name)
{
    auto source_file = GetFile(std::format("{}.cpp", module_name));

    // Import this module's header file
    source_file->Print(IncludeHeaderMacro(module_name));

    // TODO: emit local module import directives on source file
    // TODO: emit local symbol forward declarations on source file
}

void CodeGenerator::Visit(Module* node)
{
    const auto& module_name = node->ModuleName().lexeme;

    // Handle per-module actions such as header file generation.
    if (!IsModuleNodeVisited(module_name))
    {
        MarkModuleAsVisited(module_name);

        InitializeHeaderFile(module_name);
        InitializeSourceFile(module_name);
    }

    // From now on, all implementation codes will be emitted to this file.
    m_current_output_file = GetFile(std::format("{}.cpp", module_name));
}

} // namespace mylang
