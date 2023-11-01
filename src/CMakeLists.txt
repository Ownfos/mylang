add_library(mylanglib
    file/DummySourceFile.cpp
    file/SourceFile.cpp
    lexer/DummyLexicalAnalyzer.cpp
    lexer/LookAheadManager.cpp
    lexer/LexicalAnalyzer.cpp
    lexer/LexicalError.cpp
)

target_compile_features(mylanglib PUBLIC cxx_std_20)
target_include_directories(mylanglib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(mylang main.cpp)
target_link_libraries(mylang PRIVATE mylanglib)