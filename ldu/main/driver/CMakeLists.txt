set(LLVM_LINK_COMPONENTS
    Support
)

add_ldu_main(ldu Main.cpp)

target_link_libraries(ldu
        PRIVATE
        lduBasic
        lduLexer
        lduParser
        lduSema
)