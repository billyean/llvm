set(LLVM_LINK_COMPONENTS support)

add_ldu_library(lduSema
    Scope.cpp
    Sema.cpp

    LINK_LIBS
    lduBasic
)