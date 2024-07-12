set(LLVM_LINK_COMPONENTS support)

add_ldu_library(lduParser
    Parser.cpp

    LINK_LIBS
    lduBasic
    lduLexer
)