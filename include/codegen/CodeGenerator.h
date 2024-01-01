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
    virtual void Visit(FuncDecl* node) override;
    virtual void Visit(StructDecl* node) override;
    virtual void Visit(CompoundStmt* node) override;
    virtual void Visit(VarDeclStmt* node) override;
    virtual void Visit(VarInitExpr* node) override;
    virtual void Visit(VarInitList* node) override;
    virtual void Visit(ExprStmt* node) override;
    virtual void Visit(IfStmt* node) override;
    virtual void Visit(ForStmt* node) override;

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

    // This will emit "#include" for each import directive with matching "export" attribute.
    // ex) if export_qualifier is true, "import export xxx" => "#include xxx.h"
    void EmitImportDirectives(IOutputFile* output_file, const std::set<ModuleImportInfo>& import_directives, bool export_qualifier);

    // This will emit forward declaration of given symbols.
    // StructDecl => "struct [name];"
    // FuncDecl => "[return type] [name]([param1], [param2], ...);"
    void EmitForwardDecl(IOutputFile* output_file, const std::vector<Symbol>& symbols);

    ProgramEnvironment& m_environment;
    std::filesystem::path m_output_dir;
    std::unique_ptr<IOutputFileFactory> m_file_factory;
    std::map<std::string, std::shared_ptr<IOutputFile>> m_open_files;

    // These are the names of the Module nodes we've visited.
    // See IsFirstModuleNode() for more details on how we aggregate
    // module implementation split across multiple files.
    std::set<std::string> m_visited_modules;

    // The target file where generated codes are printed.
    // Except for global symbol forward declaration or import directives,
    // this will mostly be set to the source file (*.cpp).
    IOutputFile* m_current_output_file;

    // This flag, when set to true, prevents tree traversal on FuncDecl and StructDecl.
    bool m_is_forward_decl_step = false;
};

} // namespace mylang

#endif MYLANG_CODE_GENERATOR_H
