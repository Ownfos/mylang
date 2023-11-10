add_library(mylanglib
    file/DummySourceFile.cpp
    file/SourceFile.cpp
    file/ISourceFile.cpp
    lexer/DummyLexicalAnalyzer.cpp
    lexer/LexicalAnalyzer.cpp
    lexer/LexicalError.cpp
    parser/SyntaxAnalyzer.cpp
    parser/ast/Program.cpp
    parser/ast/globdecl/GlobalDecl.cpp
    parser/ast/globdecl/FuncDecl.cpp
    parser/ast/stmt/CompoundStmt.cpp
    parser/ast/visitor/TreePrinter.cpp
    parser/type/Type.cpp
    parser/type/PrimitiveType.cpp
)

target_compile_features(mylanglib PUBLIC cxx_std_20)
target_include_directories(mylanglib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(mylang main.cpp)
target_link_libraries(mylang PRIVATE mylanglib)