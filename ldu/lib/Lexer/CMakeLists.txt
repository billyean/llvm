set(LLVM_LINK_COMPONENTS support)

add_ldu_library(lduLexer
    Lexer.cpp

    LINK_LIBS
    lduBasic
)