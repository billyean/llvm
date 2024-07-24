set(LLVM_LINK_COMPONENTS support)

add_ltiger_library(ltigerSema
    Scope.cpp
    Sema.cpp

    LINK_LIBS
    ltigerBasic
)