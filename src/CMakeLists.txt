add_library(mylanglib
    file/DummySourceFile.cpp
    file/DummyOutputFile.cpp
    file/SourceFile.cpp
    file/ISourceFile.cpp
    file/DummyOutputFileFactory.cpp
    
    lexer/DummyLexicalAnalyzer.cpp
    lexer/LexicalAnalyzer.cpp
    lexer/LexicalError.cpp
    lexer/Token.cpp

    parser/ast/Module.cpp

    parser/ast/expr/ArrayAccessExpr.cpp
    parser/ast/expr/BinaryExpr.cpp
    parser/ast/expr/FuncCallExpr.cpp
    parser/ast/expr/Identifier.cpp
    parser/ast/expr/Literal.cpp
    parser/ast/expr/MemberAccessExpr.cpp
    parser/ast/expr/PostfixExpr.cpp
    parser/ast/expr/PrefixExpr.cpp

    parser/ast/globdecl/Parameter.cpp
    parser/ast/globdecl/FuncDecl.cpp
    parser/ast/globdecl/StructDecl.cpp

    parser/ast/stmt/CompoundStmt.cpp
    parser/ast/stmt/IfStmt.cpp
    parser/ast/stmt/ForStmt.cpp
    parser/ast/stmt/WhileStmt.cpp
    parser/ast/stmt/JumpStmt.cpp
    parser/ast/stmt/VarDeclStmt.cpp
    parser/ast/stmt/ExprStmt.cpp

    parser/ast/varinit/VarInitExpr.cpp
    parser/ast/varinit/VarInitList.cpp

    parser/ast/visitor/TreePrinter.cpp
    parser/ast/visitor/GlobalSymbolScanner.cpp
    parser/ast/visitor/TypeChecker.cpp
    parser/ast/visitor/JumpStmtUsageChecker.cpp

    parser/type/Type.cpp
    parser/type/base/PrimitiveType.cpp
    parser/type/base/StructType.cpp
    parser/type/base/FuncType.cpp
    parser/type/base/VoidType.cpp

    parser/routine/TypeParser.cpp
    parser/routine/ExprParser.cpp
    parser/routine/StmtParser.cpp
    parser/routine/GlobalDeclParser.cpp
    parser/routine/ModuleParser.cpp

    parser/SymbolTable.cpp
    parser/ProgramEnvironment.cpp

    parser/SyntaxAnalyzer.cpp
    parser/SyntaxError.cpp
    
    parser/SemanticError.cpp
)

target_compile_features(mylanglib PUBLIC cxx_std_20)
target_include_directories(mylanglib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(mylang main.cpp)
target_link_libraries(mylang PRIVATE mylanglib)