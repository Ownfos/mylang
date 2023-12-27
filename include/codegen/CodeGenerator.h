#ifndef MYLANG_CODE_GENERATOR_H
#define MYLANG_CODE_GENERATOR_H

#include "file/IOutputFileFactory.h"
#include "parser/ProgramEnvironment.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <map>

namespace mylang
{

class CodeGenerator : public IAbstractSyntaxTreeVisitor
{
public:
    CodeGenerator(
        ProgramEnvironment& environment,
        const std::filesystem::path& output_dir,
        std::unique_ptr<IOutputFileFactory>&& file_factory
    );
    ~CodeGenerator();

    // Returns the corresponding file instance.
    // If a file is not open yet, a new one will be created.
    IOutputFile* GetFile(const std::string& file_name);
    void CloseAllFiles();

    virtual void Visit(Module* node) override;

private:
    // Returns false if the module declaration was seen before.
    // This becomes true right after visiting a Module node with unseen module name.
    // Look at InitializeModuleFiles() for what happens if this returns false.
    bool IsModuleNodeVisited(const std::string& module_name) const;
    void MarkModuleAsVisited(const std::string& module_name);

    // This function gets executed right after we encounter an unseen Module node.
    // In this routine, we need to do all jobs that need to be done only once per module.
    // Remember that a single module can be implemented in multiple files!
    // 
    // Things to handle:
    // 1. Create a header file for this node with a header guard
    // 2. Emit #include for public module import directives (i.e., "import export xxx")
    // 3. Emit global symbol forward declarations (i.e., functions and structs with "expor" prefix)
    // 4. Create a source file for this node
    // 5. Emit #include for private module import directives (i.e., "import xxx")
    // 6. Emit local symbol forward declarations (i.e., functions and structs without "export" prefix)
    void InitializeHeaderFile(const std::string& module_name);
    void InitializeSourceFile(const std::string& module_name);

    ProgramEnvironment& m_environment;
    std::filesystem::path m_output_dir;
    std::unique_ptr<IOutputFileFactory> m_file_factory;
    std::map<std::string, std::shared_ptr<IOutputFile>> m_open_files;

    // These are the names of the Module nodes we've visited.
    // See IsFirstModuleNode() for more details on how we aggregate
    // module implementation split across multiple files.
    std::set<std::string> m_visited_modules;

    // The "xxx.cpp" file on which we will emit all implementation code.
    IOutputFile* m_current_output_file;
};

} // namespace mylang

#endif MYLANG_CODE_GENERATOR_H
