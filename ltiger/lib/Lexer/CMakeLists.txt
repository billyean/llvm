set(LLVM_LINK_COMPONENTS support)

add_ltiger_library(ltigerLexer
        Lexer.cpp
        LINK_LIBS
        ltigerBasic
)