project(tigerc)

# minimum required CMAKE version
CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# compiler must be 17
set(CMAKE_CXX_STANDARD 17)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the libary source.
# set(ANTLR4_TAG 4.12.0)
# set(ANTLR4_ZIP_REPOSITORY https://github.com/antlr/antlr4/archive/refs/tags/4.12.0.zip)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE /home/haibo/antlr-4.12.0-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
# Call macro to add lexer and grammar to your build dependencies.
antlr_target(TigerGrammarLexer TigerLexer.g4 LEXER
        PACKAGE antlrcpptest)
antlr_target(TigerGrammarParser TigerParser.g4 PARSER
        PACKAGE antlrcpptest
        DEPENDS_ANTLR TigerGrammarLexer
        COMPILE_FLAGS -lib ${ANTLR_TigerGrammarLexer_OUTPUT_DIR})

# include generated files in project environment
include_directories(${ANTLR_TigerGrammarLexer_OUTPUT_DIR})
include_directories(${ANTLR_TigerGrammarParser_OUTPUT_DIR})

# add generated grammar to demo binary target
add_executable(${PROJECT_NAME} main.cpp
        ${ANTLR_TigerGrammarLexer_CXX_OUTPUTS}
        ${ANTLR_TigerGrammarParser_CXX_OUTPUTS})
target_link_libraries(${PROJECT_NAME} antlr4_static)
#add_executable(mytest mytest.h mytest.cpp)