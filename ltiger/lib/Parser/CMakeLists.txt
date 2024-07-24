set(LLVM_LINK_COMPONENTS support)

add_ltiger_library(ltigerParser
        Parser.cpp

        LINK_LIBS
        ltigerBasic
        ltigerLexer
        ltigerSema
)